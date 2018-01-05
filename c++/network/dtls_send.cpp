/*
	DTLS sender. Sends packet(s) to destination
	Data to send can be provided as:
		- text message
		- hexadecimal dump
		- binary file. If several files specified, each will be send in a separate packet
		- generated automatically using passed IP packet size

	Allows to set specific socket options and sending repetitions

	Created by: Maxim Klimovich
*/

#include <stdio.h>
#include <stdlib.h>
#include <list>

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <openssl/err.h>
#include <openssl/dh.h>
#include <openssl/ssl.h>
#include <openssl/conf.h>
#include <openssl/engine.h>

#define DTLSC_CERT "../../cert/dtlsc.pem"
#define DTLSC_KEY_CERT "../../cert/dtlsc.key"
#define DTLSC_ROOT_CACERT "../../cert/dtlsCA.pem"

#define MAX_STR_SIZE	128
#define MAX_PACKET_SIZE	9000

#ifdef WIN32
#pragma comment(lib, "libeay32MTd.lib")
#pragma comment(lib, "ssleay32MTd.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996) 
#endif

// Logging
#define LL_CRITICAL	0
#define LL_ERROR	1
#define LL_WARNING	2
#define LL_INFO		3
#define LL_DUMP		4
#define LL_DTLS		5

#define MAX_LOGLINE_SIZE	256

int logFilterLevel = LL_INFO;
//

// Vars for keeping parameters
char host[128];
int port;
int option = -1;
int optionVal = 0;
bool destProvided = false;
int repeatCnt = 1;
int waitTime = 0;
int localPort = 0;

struct Packet
{
	char *data;
	int len;
};
std::list<Packet *>	packets;


void showSyntax() {
	printf( "\ndtls_send. (Build 1)\n" );
	printf( "Sends DTLS packet\n\n" );
	printf( "Syntax: \n"
		"udp_send.exe -d <destination_host:destination_port>\n"
		"             [-l <local port>]\n"
		"             [-m <text message to send>] | [-b <hex buffer to be send>] | [-s <IP packet size>] | [-f \"<list of binary files>\"]\n"
		"             [-r <repeat time>]\n"
		"             [-w <wait between repetitions, sec>]\n"
		"             [-o <socket_option=value>]\n" );
	printf( "\ndtls_send.exe -? to get help: \n" );
}


/**
 * Reads passed parameters and fills in packet list to be send and other 
 * sending parameters
 */
bool readCLIParameters( int argc, char* argv[] )
{
	for (int i = 1; i < argc; i++)
	{
		const char *arg = argv[i + 1];
		int argLen = strlen( arg );

		if (( strcmp( argv[i], "-?" ) == 0 ) || ( strcmp( argv[i], "-h" ) == 0 ))
		{
			showSyntax();
			printf( "Linux IP options list:\n"
				"\tIP_MTU_DISCOVER	10\n" );

			printf( "IP_MTU_DISCOVER values\n"
				"\tIP_PMTUDISC_DONT    0\n"
				"\tIP_PMTUDISC_WANT    1\n"
				"\tIP_PMTUDISC_DO      2\n"
				"\tIP_PMTUDISC_PROBE   3\n" );

			exit( 0 );
		}

		if (strcmp( argv[i], "-d" ) == 0)
		{
			int pos = 0;
			while (( pos < argLen ) && ( arg[pos] != ':' ))
			{
				host[pos] = argv[i + 1][pos];
				pos++;
			}
			host[pos] = 0;
			if (( pos == argLen ) || ( arg[pos] != ':' ))
			{
				printf( "Port is missing\n" );
				return false;
			}
			port = atoi( &arg[++pos] );
			if (port == 0)
			{
				printf( "Port is missing\n" );
				return false;
			}
			printf( "Host = %s Port = %i\n", host, port );
			destProvided = true;
			i++;
		}
		else if (strcmp( argv[i], "-l" ) == 0)
		{
			localPort = atoi( argv[i + 1] );
			printf( "Local port = %d\n", localPort );
			i++;
		}
		else if (strcmp( argv[i], "-m" ) == 0)
		{
			printf( "Message to send = %s\n", argv[i + 1] );
			int packetSize = strlen( argv[i + 1] );

			Packet *packet = new Packet;
			packet->data = new char[packetSize];
			packet->len = packetSize;
			memcpy( packet->data, argv[i + 1], packetSize );
			packets.push_back( packet );

			i++;
		}
		else if (strcmp( argv[i], "-b" ) == 0)
		{
			char *pBuf = new char[argLen + 1];
			int argPos = 0;
			int bufPos = 0;
			while (argPos < argLen)
			{
				if (isxdigit( arg[argPos] )) pBuf[bufPos++] = arg[argPos];
				argPos++;
			}
			pBuf[bufPos] = 0;
			if (bufPos % 2 == 1)
			{
				printf( "Buffer should have even number of hex digits\n" );
				return false;
			}
			int bufLen = bufPos;
			printf( "Buffer = %s\n", pBuf );
			i++;

			int packetSize = bufLen / 2;
			Packet *packet = new Packet;
			packet->data = new char[packetSize];
			packet->len = packetSize;
			for (int i = 0; i < bufLen / 2; i++)
			{
				packet->data[i] = ( ( pBuf[i * 2] - 48 ) << 4 ) | ( pBuf[i * 2 + 1] - 48 );
			}
			packets.push_back( packet );
		}
		else if (strcmp( argv[i], "-s" ) == 0)
		{
			int packetSize = atoi( argv[i + 1] );
			printf( "IP packet size = %d\n", packetSize );
			i++;

			Packet *packet = new Packet;
			packet->data = new char[packetSize];
			packet->len = packetSize;
			memset( packet->data, 0xff, packetSize );
			packets.push_back( packet );
		}
		else if (strcmp( argv[i], "-f" ) == 0)
		{
			const char *pos = arg;
			int fileNameLen = 0;
			char fileName[MAX_STR_SIZE];
			while (*pos != 0)
			{
				if (!isspace( pos[0] ))
				{
					fileName[fileNameLen++] = *pos;
				}

				if (( isspace( pos[1] ) ) || ( pos[1] == 0 ))
				{
					if (fileNameLen > 0)
					{
						fileName[fileNameLen] = 0;
						printf( "Read binary file: %s\n", fileName );
						FILE *f = fopen( fileName, "rb" );
						if (f == NULL)
						{
							printf( "fopen() error. Cannot open file\n" );
							return false;
						}
						char *buf = new char[MAX_PACKET_SIZE];
						int readSize = fread( buf, 1, MAX_PACKET_SIZE, f );
						if (readSize <= 0)
						{
							printf( "Nothing is read from file\n" );
							return false;
						}
						else
						{
							Packet *packet = new Packet;
							packet->data = new char[readSize];
							packet->len = readSize;
							memcpy( packet->data, buf, readSize );
							packets.push_back( packet );
						}
						fileNameLen = 0;
					}
				}
				++pos;
			}
			i++;
		}
		else if (strcmp( argv[i], "-r" ) == 0)
		{
			repeatCnt = atoi( argv[i + 1] );
			printf( "Number to repeat = %d\n", repeatCnt );
			i++;
		}
		else if (strcmp( argv[i], "-w" ) == 0)
		{
			waitTime = atoi( argv[i + 1] );
			printf( "Time to wait = %d\n", waitTime );
			i++;
		}
		else if (strcmp( argv[i], "-o" ) == 0)
		{
			char *ptr;
			option = strtol( arg, &ptr, 10 );
			if (*ptr != '=')
			{
				printf( "Option value is not provided\n" );
				return false;
			}
			optionVal = strtol( ++ptr, &ptr, 10 );
			printf( "IP socket option to be set : %d = %d\n", option, optionVal );
			i++;
		}
		else
		{
			printf( "Wrong argument: %s\n", argv[i] );
			return false;
		}
	}
	return true;
}


/*
 * Prints formatted logging output. Applies severity filter
 */
void log( int logLevel, int ndx, const char *format, ... )
{
	char buf[MAX_LOGLINE_SIZE];
	if (logLevel > logFilterLevel) return;
	switch (logLevel) {
	case LL_CRITICAL: sprintf( buf, "CRITICAL  [%02d] ", ndx ); break;
	case LL_ERROR:    sprintf( buf, "ERROR     [%02d] ", ndx ); break;
	case LL_WARNING:  sprintf( buf, "WARNING   [%02d] ", ndx ); break;
	case LL_INFO:     sprintf( buf, "INFO      [%02d] ", ndx ); break;
	case LL_DUMP:     sprintf( buf, "DUMP      [%02d] ", ndx ); break;
	case LL_DTLS:     sprintf( buf, "DTLS DUMP [%02d] ", ndx ); break;
	}

	char *strPtr = buf + strlen( buf );
	va_list args;
	va_start( args, format );
	vsprintf( strPtr, format, args );
	va_end( args );

	printf( buf );
}


/**
 *	Callback that verifies certificate
 *
 *	@returns 1 if verification is passed
 */
int verifyCert( int ok, X509_STORE_CTX *ctx )
{
	log( LL_DUMP, 0, "verify_cert() is called. Returning OK\n" );
	return 1;
}


/**
 *	Callback that prints debugging info such as state information for SSL objects
 *	created from ctx during connection setup and use.
 */
void userSSLInfoCallback( const SSL *ssl, int where, int ret )
{
	const char *str = NULL;
	int w;

	w = where & ~SSL_ST_MASK;

	str = where & SSL_ST_CONNECT ? "connect" : where & SSL_ST_ACCEPT ? "accept" : "undefined";
	if (where & SSL_CB_LOOP)
	{
		log( LL_DTLS, 0, "		SSL state [\"%s\"]: %s\n", str, SSL_state_string_long( ssl ) );
	}
	else if (where & SSL_CB_ALERT)
	{
		log( LL_DTLS, 0, "		SSL: alert [\"%s\"]: %s : %s\n", where & SSL_CB_READ ? "read" : "write", \
			SSL_alert_type_string_long( ret ), SSL_alert_desc_string_long( ret ) );
	}
	else if (where & SSL_CB_EXIT)
	{
		if (ret == 0)
			log( LL_DTLS, 0, "		SSL state [\"%s\"]: failed in %s\n", str, SSL_state_string_long( ssl ) );
		else if (ret < 0)
			log( LL_DTLS, 0, "		SSL state [\"%s\"]: error in %s\n", str, SSL_state_string_long( ssl ) );
	}
}


/**
 *	Main func. Sends a message to server:port using DTLS transport
 *
 *	@param server
 *	@param port
 */
int main( int argc, char *argv[] )
{
	if (!readCLIParameters( argc, argv ) || !destProvided)
	{
		showSyntax();
		return -1;
	}

	// Init SSL library
	SSL_library_init ();
	ERR_clear_error ();
    ERR_load_BIO_strings();
	SSL_load_error_strings ();
	OpenSSL_add_all_algorithms ();

    SSL_CTX *ctx;
    SSL *ssl;
    BIO *bio;
    int result;

    // Initialize the DTLS context 
	ctx = SSL_CTX_new(DTLSv1_client_method());
	SSL_CTX_set_options( ctx, SSL_OP_NO_QUERY_MTU );

	// Load certificate
	if (!SSL_CTX_use_certificate_chain_file( ctx, DTLSC_CERT ))
	{
		fprintf( stderr, "SSL_CTX_use_certificate_chain_file() error: %s", strerror(errno) );
		return -1;
	}

	// Load private key
	if (!SSL_CTX_use_PrivateKey_file( ctx, DTLSC_KEY_CERT, SSL_FILETYPE_PEM ))
	{
		fprintf( stderr, "SSL_CTX_use_PrivateKey_file() error: %s", strerror(errno) );
		return -1;
	}

	if (!SSL_CTX_load_verify_locations( ctx, DTLSC_ROOT_CACERT, 0 ))
	{
		fprintf( stderr, "SSL_CTX_load_verify_locations() error: %s", strerror(errno) );
		return -1;
	}
	
    // Check if the private key is valid
    result = SSL_CTX_check_private_key( ctx );
    if (result != 1) {
		fprintf( stderr, "Error: checking the private key failed. \n" );
		return -1;
    }

	// Configure read ahead mode is ON (read all available bytes at once). Must be set
	SSL_CTX_set_read_ahead( ctx, 1 );

	// Configure callback func that will print state information for SSL objects 
	// created from ctx during connection setup and use.
	SSL_CTX_set_info_callback( ctx, &userSSLInfoCallback );

#ifdef WIN32
	// Initialize socket library
	WSADATA wsaData;
	if (WSAStartup( MAKEWORD( 1, 1 ), &wsaData ) != 0) {
		fprintf( stderr, "WSAStartup() failed\n" );
		return -1;
	}
	typedef int socklen_t;
#endif

	// Create connected UDP socket
    struct sockaddr_in serverAddr;
	memset((void *)&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons( port );
	serverAddr.sin_addr.s_addr = inet_addr( host );

	int sock = socket( PF_INET, SOCK_DGRAM, 0 );

	// bind socket to local client's port
	struct sockaddr_in localAddr;
	memset( &localAddr, 0, sizeof( struct sockaddr_in ) );
	localAddr.sin_family = AF_INET;
	localAddr.sin_port = htons( localPort );

	if (bind( sock, ( struct sockaddr * )&localAddr, sizeof( localAddr ) ) == -1)
	{
		fprintf( stderr, "bind() error: %s\n", strerror( errno ) );
		return -1;
	}

	// Connect UDP socket
	if (connect( sock, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in) ) == -1)
	{
		fprintf( stderr, "connect() error: %s", strerror(errno) );
		return -1;
	}

	// Get peer address
	struct sockaddr peer;
	int peerlen = sizeof( struct sockaddr );
	if (getsockname( sock, &peer, (socklen_t *)&peerlen) < 0)
	{
		fprintf( stderr, "getsockname() error: %s\n", strerror( errno ) );
		return -1;
	}

	// Prepare SSL and BIO
	ssl = SSL_new(ctx);
	// Set SSL-level MTU for _HANDSHAKE_ messages (data messages are not affected)
	SSL_set_mtu( ssl, 9000 );

	bio = BIO_new_dgram( sock, BIO_NOCLOSE );
	BIO_ctrl_set_connected( bio, 1, &peer );
	SSL_set_bio( ssl, bio, bio );
	SSL_set_connect_state( ssl );

	// Establish SSL connection
	if (!SSL_connect( ssl ))
	{
		fprintf( stderr, "SSL_connect() error: %s", strerror( errno ) );
		return -1;
	}

	// Send message(s) to peer
	printf( "\n" );

	int packetCnt = packets.size();
	if (packetCnt == 0)
	{
		printf( "Nothing to send\n" );
		return -1;
	}

	for (int i = 0; i < repeatCnt; i++)
	{
		if (repeatCnt > 1)
		{
			printf( "Repetition %d is started\n", i + 1 );
		}

		int cnt = 1;
		for (Packet *p : packets)
		{
			printf( "Sending packet %d of %d\n", cnt++, packetCnt );
			int sendSize = SSL_write( ssl, p->data, p->len );
			if (sendSize != p->len)
			{
				fprintf( stderr, "SSL_write() failed to write the entire buffer: %s\n", strerror( errno ) );
			}
		}

		if (repeatCnt > 1)
		{
#ifdef WIN32
			Sleep( waitTime * 1000 );
#else
			sleep( waitTime );
#endif
		}
	}
	printf( "Done\n" );

    // Teardown the link and context state.
	SSL_shutdown( ssl );
    SSL_free( ssl );
    SSL_CTX_free( ctx );
}
