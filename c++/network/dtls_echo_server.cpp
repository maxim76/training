/*
	Single-thread blocking DTLS echo-server. 
	Opens DTLS port and listens for new connections.
	Echoes back received data.
	Supports only 1 client connection per time.
	Uses connected UDP sockets for every client
	Works on Linux, does not work on Windows becasue windows cannot distribute
	incoming data packets in correct way between all connected socket that 
	are sharing the same port number

	Created by: Maxim Klimovich
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#include <fcntl.h>

#include "openssl/err.h"
#include "openssl/dh.h"
#include "openssl/ssl.h"
#include "openssl/conf.h"
#include "openssl/engine.h"

#define DTLS_CERT "../../cert/dtlsc.pem"
#define DTLS_KEY_CERT "../../cert/dtlsc.key"
#define DTLS_ROOT_CACERT "../../cert/dtlsCA.pem"


#define COOKIE_LEN  20

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

#define BUF_SIZE		4096
#define MAX_LOGLINE_SIZE	BUF_SIZE + 256

int logFilterLevel = LL_INFO;
//

/*
BIO *bio_err = NULL;
BIO *bio_s_out = NULL;
BIO *dtls_bio_err = 0;
*/


/*
* Prints formatted logging output. Applies severity filter
*/
void log( int logLevel, int ndx, const char *format, ... )
{
	char buf[MAX_LOGLINE_SIZE];
	if (logLevel > logFilterLevel) return;
	switch (logLevel) {
	case LL_CRITICAL: sprintf( buf, "CRITICAL  [%03d] ", ndx ); break;
	case LL_ERROR:    sprintf( buf, "ERROR     [%03d] ", ndx ); break;
	case LL_WARNING:  sprintf( buf, "WARNING   [%03d] ", ndx ); break;
	case LL_INFO:     sprintf( buf, "INFO      [%03d] ", ndx ); break;
	case LL_DUMP:     sprintf( buf, "DUMP      [%03d] ", ndx ); break;
	case LL_DTLS:     sprintf( buf, "DTLS DUMP [%03d] ", ndx ); break;
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
	log( LL_DUMP, 0, "verifyCert() is called. Returning OK\n" );
	return 1;
}


/**
*	Callback that verifies cookies received back from client
*
*	@returns 1 if verification is passed
*/
int verifyCookie( SSL *ssl, unsigned char *cookie, unsigned int cookieLen )
{
	log( LL_DUMP, 0, "verifyCookie() is called. Returning OK\n" );
	return 1;
}


/**
*	Callback that generates cookie to be inserted into "Hello Verify Request"
*
*	@returns 1 if verification is passed
*/
int generateCookie( SSL *ssl, unsigned char *cookie, unsigned int *cookieLen )
{
	log( LL_DUMP, 0, "generateCookie() is called. Returning OK\n" );
	unsigned int i;

	for (i = 0; i < COOKIE_LEN; i++, cookie++)
		*cookie = i;
	*cookieLen = COOKIE_LEN;

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


/*
* Decodes and prints SSL error
*/
void printSSLError( SSL *ssl, int res )
{
	switch (SSL_get_error( ssl, res ))
	{
	case SSL_ERROR_NONE:
		log( LL_DTLS, 0, "SSL_ERROR_NONE\n" );
		break;
	case SSL_ERROR_WANT_WRITE:
		log( LL_DTLS, 0, "SSL_ERROR_WANT_WRITE\n" );
		break;
	case SSL_ERROR_WANT_READ:
		log( LL_DTLS, 0, "SSL_ERROR_WANT_READ\n" );
		break;
	case SSL_ERROR_WANT_X509_LOOKUP:
		log( LL_DTLS, 0, "SSL_ERROR_WANT_X509_LOOKUP\n" );
		break;
	case SSL_ERROR_SYSCALL:
		log( LL_DTLS, 0, "SSL_ERROR_WANT_X509_LOOKUP\n" );
		break;
	case SSL_ERROR_SSL:
		log( LL_DTLS, 0, "SSL_ERROR_WANT_X509_LOOKUP\n" );
		break;
	case SSL_ERROR_ZERO_RETURN:
		log( LL_DTLS, 0, "SSL_ERROR_WANT_X509_LOOKUP\n" );
		break;
	default:
		log( LL_DTLS, 0, "\nOTHER\n" );
	}
}


void printSSLDetails( SSL *ssl )
{
	BIO *rBIO = SSL_get_rbio( ssl );
	BIO *wBIO = SSL_get_wbio( ssl );
	int tmp;

	printf( "SSL: %u, rBIO: %u, wBIO: %u  rBIO FD: %u   wBIO FD: %u\n",
		(long)ssl, (long)rBIO, (long)wBIO,
		BIO_get_fd( rBIO, &tmp ), BIO_get_fd( wBIO, &tmp ) );
}


template <typename T>
int setSocketOption( int socket, int level, int optname, const T & optval )
{
#if defined( _WIN32 )
	const char * optvalCast = reinterpret_cast< const char * >( &optval );
#else
	const void * optvalCast = &optval;
#endif
	return setsockopt( socket, level, optname, optvalCast, sizeof( T ) );
}



/*
 * Struct keeps secure session properties
 */
struct TSession {
	SSL *ssl;
	BIO *bio;
	struct sockaddr_in clientAddr;
	int clientSocket;
};


/*
* Creates data struct that keeps session info
*/
TSession *newSession( SSL_CTX *ctx, int sock )
{
	TSession *pSession = new TSession();
	memset( (char *)&pSession->clientAddr, 0, sizeof( struct sockaddr_in ) );

	pSession->ssl = SSL_new( ctx );
	SSL_clear( pSession->ssl );
	SSL_set_options( pSession->ssl, SSL_OP_COOKIE_EXCHANGE );

	pSession->bio = BIO_new_dgram( sock, BIO_NOCLOSE );

	BIO_ctrl( pSession->bio, BIO_CTRL_DGRAM_MTU_DISCOVER, 0, NULL );

	//BIO_set_nbio( pSession->bio, 1 );
	SSL_set_bio( pSession->ssl, pSession->bio, pSession->bio );
	SSL_set_accept_state( pSession->ssl );

	//BIO_ctrl( pSession->bio, BIO_CTRL_DGRAM_SET_MTU, 1300, NULL );
	//SSL_set_mtu( pSession->ssl, 1300 );
	//DTLS_set_link_mtu( pSession->ssl, 1300 );

	return pSession;
}

void terminateSession( TSession *pSession )
{
	SSL_shutdown( pSession->ssl );
	SSL_free( pSession->ssl );
	delete pSession;
}

/*
* Creates connected UDP socket with client
*/
bool createClientSocket( TSession *pSession, sockaddr_in *pServerAddr )
{
	// create connected UDP socket for client
	pSession->clientSocket = socket( PF_INET, SOCK_DGRAM, 0 );
	if (setSocketOption<int>( pSession->clientSocket, SOL_SOCKET, SO_REUSEADDR, 1 ) == -1)
	{
		log( LL_ERROR, (int)pSession->clientSocket, "client setsockopt(SO_REUSEADDR) error: %s\n", strerror( errno ) );
		//close( pSession->clientSocket );
		return false;
	}
#ifndef WIN32
	if (setSocketOption<int>( pSession->clientSocket, SOL_SOCKET, SO_REUSEPORT, 1 ) == -1)
	{
		log( LL_ERROR, (int)pSession->clientSocket, "client setsockopt(SO_REUSEPORT) error: %s\n", strerror( errno ) );
		//close( pSession->clientSocket );
		return false;
	}
#endif

	if (( bind( pSession->clientSocket, ( struct sockaddr * )pServerAddr, sizeof( struct sockaddr_in ) ) ) == -1)
	{
		log( LL_ERROR, (int)pSession->clientSocket, "client bind() error: %s\n", strerror( errno ) );
		//close( pSession->clientSocket );
		return false;
	}
	if (( connect( pSession->clientSocket, ( struct sockaddr * )&( pSession->clientAddr ), sizeof( struct sockaddr_in ) ) ) == -1)
	{
		log( LL_ERROR, (int)pSession->clientSocket, "client connect() error: %s\n", strerror( errno ) );
		//close( pSession->clientSocket );
		return false;
	}
	return true;
}


/**
*	Main func. Opens port and waiting for connection using DTLS transport. Echoes back received messages
*
*	@param server
*	@param port
*/
int main( int argc, char *argv[] )
{
	if (argc < 3)
	{
		fprintf( stderr, "Usage: %s <server_ip> <server_port> [<log_level>]\n", argv[0] );
		return -1;
	}

	int serverPort = atoi( argv[2] );
	if (argc == 4) logFilterLevel = atoi( argv[3] );

	log( LL_INFO, 0, "Single-thread blocking DTLS echo-server\n\n" );

	// Init SSL library
	SSL_library_init();
	ERR_clear_error();
	ERR_load_BIO_strings();
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();

	SSL_CTX *ctx;
	int result;

	// Initialize the DTLS context 
	ctx = SSL_CTX_new( DTLSv1_server_method() );
	if (ctx == NULL)
	{
		fprintf( stderr, "SSL_CTX_new() failed\n" );
		return -1;
	}
	SSL_CTX_set_quiet_shutdown( ctx, 1 );

	// Load certificate
	if (!SSL_CTX_use_certificate_chain_file( ctx, DTLS_CERT ))
	{
		fprintf( stderr, "SSL_CTX_use_certificate_chain_file() error: %s\n", strerror( errno ) );
		return -1;
	}

	// Load private key
	if (!SSL_CTX_use_PrivateKey_file( ctx, DTLS_KEY_CERT, SSL_FILETYPE_PEM ))
	{
		fprintf( stderr, "SSL_CTX_use_PrivateKey_file() error: %s\n", strerror( errno ) );
		return -1;
	}

	// Check if the private key is valid
	result = SSL_CTX_check_private_key( ctx );
	if (result != 1) {
		fprintf( stderr, "Error: checking the private key failed. \n" );
		return -1;
	}

	// Set cookie generate callback
	SSL_CTX_set_cookie_generate_cb( ctx, generateCookie );
	// Set cookie verfy callback
	SSL_CTX_set_cookie_verify_cb( ctx, verifyCookie );
	// Set verify server's certificate callback
	SSL_CTX_set_verify( ctx, SSL_VERIFY_CLIENT_ONCE, verifyCert );

	// Configure callback func that will print state information for SSL objects created from ctx 
	// during connection setup and use.
	SSL_CTX_set_info_callback( ctx, &userSSLInfoCallback );

	// Configure read ahead mode is ON (read all available bytes at once). Must be set
	SSL_CTX_set_read_ahead( ctx, 1 );


#ifdef WIN32
	// Initialize socket library
	WSADATA wsaData;
	if (WSAStartup( MAKEWORD( 1, 1 ), &wsaData ) != 0) {
		fprintf( stderr, "WSAStartup() failed\n" );
		return -1;
	}
#endif

	// Open normal UDP socket and bind to server:port
	struct sockaddr_in serverAddr;
	memset( (void *)&serverAddr, 0, sizeof( struct sockaddr_in ) );
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons( serverPort );
	serverAddr.sin_addr.s_addr = inet_addr( argv[1] );

	int listeningSocket;
	if (( listeningSocket = socket( PF_INET, SOCK_DGRAM, 0 ) ) == -1)
	{
		fprintf( stderr, "socket() error: %s\n", strerror( errno ) );
		return -1;
	}

	// Do bind before setting SO_REUSEPORT. Seems it works too despite what is said in man
	// Early bind (before port is shared) lets ensure that no other apps use this port
	if (( bind( listeningSocket, ( struct sockaddr * )&serverAddr, sizeof( struct sockaddr_in ) ) ) == -1)
	{
		fprintf( stderr, "bind() error: %s\n", strerror( errno ) );
		return -1;
	}

	if (setSocketOption<int>( listeningSocket, SOL_SOCKET, SO_REUSEADDR, 1 ) == -1)
	{
		fprintf( stderr, "setsockopt( SO_REUSEADDR ) error: %s\n", strerror( errno ) );
		return -1;
	}
#ifndef WIN32
	if (setSocketOption<int>( listeningSocket, SOL_SOCKET, SO_REUSEPORT, 1 ) == -1)
	{
		fprintf( stderr, "setsockopt( SO_REUSEPORT ) error: %s\n", strerror( errno ) );
		return -1;
	}
#endif
	
	/*
	// TODO: Cannot set this option. Need to find out why
	if (setSocketOption<int>( listeningSocket, IPPROTO_IP, IP_DONTFRAGMENT, 1 ) == -1)
	{
		fprintf( stderr, "setsockopt( IP_DONTFRAGMENT ) error: %s\n", strerror( errno ) );
		return -1;
	}
	*/

	/*
	// Here where bind() is supposed to be according to man page (after SO_REUSEPORT is set)
	if (( bind( listeningSocket, ( struct sockaddr * )&serverAddr, sizeof( struct sockaddr_in ) ) ) == -1)
	{
		fprintf( stderr, "bind() error: %s\n", strerror( errno ) );
		return -1;
	}
	*/
	log( LL_INFO, 0, "INFO: Starting Server On %s:%d\n", inet_ntoa( serverAddr.sin_addr ), ntohs( serverAddr.sin_port ) );

	TSession *pSession;

	/*
		Main cycle: create new session, wait for connection, read data and echo back until client close connection, delete session.
	*/
	while(1)
	{
		pSession = newSession( ctx, listeningSocket );

		log( LL_INFO, 0, "Listen for income connection...\n" );
		int listenStatus = 0;
		int status;
		while (listenStatus == 0)
		{
			//printf( "BEFORE DTLSv1_listen(). Press ENTER\n" );getchar();
			listenStatus = DTLSv1_listen( pSession->ssl, ( struct sockaddr * )&( pSession->clientAddr ) );
		}
		if (listenStatus < 0)	// fatal DTLSv1_listen error
		{
			log( LL_DUMP, 0, "DTLSv1_listen() returned %d\n", listenStatus );
			terminateSession( pSession );
			continue;
		}
		log( LL_INFO, 0, "New connection offered %s:%d\n", inet_ntoa( pSession->clientAddr.sin_addr ), ntohs( pSession->clientAddr.sin_port ) );
		if (createClientSocket( pSession, &serverAddr ))
		{
			// Replace listeningSocket with clientSocket in the BIO object
			printSSLDetails( pSession->ssl );

			//BIO_set_fd( SSL_get_rbio( pSession->ssl ), pSession->clientSocket, BIO_NOCLOSE );
			//BIO_ctrl( SSL_get_rbio( pSession->ssl ), BIO_CTRL_DGRAM_SET_CONNECTED, 0, &( pSession->clientAddr ) );
			BIO *clientBIO = SSL_get_rbio( pSession->ssl );
			BIO_set_fd( clientBIO, pSession->clientSocket, BIO_NOCLOSE );
			BIO_ctrl( clientBIO, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &( pSession->clientAddr ) );

			/*
			BIO_set_fd( SSL_get_wbio( pSession->ssl ), pSession->clientSocket, BIO_NOCLOSE );
			BIO_ctrl( SSL_get_wbio( pSession->ssl ), BIO_CTRL_DGRAM_SET_CONNECTED, 0, &( pSession->clientAddr ) );
			*/

			printSSLDetails( pSession->ssl );

			// Accept the connection
			log( LL_DUMP, 0, "Before SSL_accept\n" );
			status = SSL_accept( pSession->ssl );
			log( LL_DUMP, 0, "After SSL_accept\n" );
			if (status != 1) {
				log( LL_WARNING, 0, "SSL_accept() returned %d\n", status );
				printSSLError( pSession->ssl, status );
				terminateSession( pSession );
				continue;
			}
			log( LL_DUMP, 0, "Connection is accepted\n" );

			// Start read/reply cycle
			bool clientConnected = true;
			while (clientConnected)
			{
				// Receive message
				char buffer[BUF_SIZE];
				int len = SSL_read( pSession->ssl, buffer, sizeof( buffer ) );

				switch (SSL_get_error( pSession->ssl, len ))
				{
				case SSL_ERROR_NONE:
					buffer[len] = 0;
					log( LL_INFO, (int)pSession->clientSocket, "%s:%d IN : %s\n", inet_ntoa( pSession->clientAddr.sin_addr ), ntohs( pSession->clientAddr.sin_port ), buffer );

					// Send echo responce
					char echoBuffer[BUF_SIZE];
					strncpy( echoBuffer, "Server Echo: ", BUF_SIZE );
					strncat( echoBuffer, buffer, BUF_SIZE );
					len = SSL_write( pSession->ssl, echoBuffer, strlen( echoBuffer ) );
					if (strlen( echoBuffer ) != len) {
						log( LL_WARNING, (int)pSession->clientSocket, "Failed to write the entire buffer to %s:%d. Connection closed\n",
							inet_ntoa( pSession->clientAddr.sin_addr ), ntohs( pSession->clientAddr.sin_port ) );
						break;
					}
					else
					{
						log( LL_INFO, (int)pSession->clientSocket, "%s:%d OUT: %s\n", inet_ntoa( pSession->clientAddr.sin_addr ), ntohs( pSession->clientAddr.sin_port ), echoBuffer );
					}

				case SSL_ERROR_WANT_WRITE:
				case SSL_ERROR_WANT_READ:
				case SSL_ERROR_WANT_X509_LOOKUP:
					log( LL_DUMP, (int)pSession->clientSocket, "Read BLOCK\n" );
					break;
				case SSL_ERROR_SYSCALL:
				case SSL_ERROR_SSL:
					log( LL_DUMP, (int)pSession->clientSocket, "ERROR\n" );
					break;
				case SSL_ERROR_ZERO_RETURN:
					log( LL_DUMP, (int)pSession->clientSocket, "DONE\n" );
					log( LL_INFO, (int)pSession->clientSocket, "%s:%d Connection closed\n", inet_ntoa( pSession->clientAddr.sin_addr ), ntohs( pSession->clientAddr.sin_port ) );
					//close( pSession->clientSocket );
					clientConnected = false;
					break;
				default:
					log( LL_DUMP, (int)pSession->clientSocket, "OTHER\n" );
				}
			} // while (clientConnected)
		}
		else
		{
			log( LL_WARNING, 0, "Could not create client socket connection\n" );
		}

		terminateSession( pSession );
	} // Main cycle
}
