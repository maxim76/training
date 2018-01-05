/*
	Single-thread blocking DTLS echo-server. 
	Opens DTLS port and listens for new connections.
	Echoes back received data.
	Supports only 1 client connection per time.
	Uses listeningSocket for all communications. Works on Linux and Windows

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


#define COOKIE_LEN  16

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

	if (( bind( listeningSocket, ( struct sockaddr * )&serverAddr, sizeof( struct sockaddr_in ) ) ) == -1)
	{
		fprintf( stderr, "bind() error: %s\n", strerror( errno ) );
		return -1;
	}

	log( LL_INFO, 0, "INFO: Starting Server On %s:%d\n", inet_ntoa( serverAddr.sin_addr ), ntohs( serverAddr.sin_port ) );


	/*
		Main cycle: create new session, wait for connection, read data and echo back until client close connection, delete session.
	*/
	while(1)
	{
		SSL *ssl = NULL;
		BIO *sbio = NULL;

		ssl = SSL_new( ctx );
		SSL_clear( ssl );
		SSL_set_options( ssl, SSL_OP_COOKIE_EXCHANGE );

		sbio = BIO_new_dgram( listeningSocket, BIO_NOCLOSE );
		BIO_ctrl( sbio, BIO_CTRL_DGRAM_MTU_DISCOVER, 0, NULL );

		SSL_set_bio( ssl, sbio, sbio );
		SSL_set_accept_state( ssl );

		log( LL_INFO, 0, "Listen for income connection...\n" );
		int listenStatus = 0;
		int status;
		struct sockaddr_in clientAddr;

		while (listenStatus == 0)
		{
			//printf( "BEFORE DTLSv1_listen(). Press ENTER\n" );getchar();
			listenStatus = DTLSv1_listen( ssl, ( struct sockaddr * )&( clientAddr ) );
		}
		if (listenStatus < 0)	// fatal DTLSv1_listen error
		{
			log( LL_DUMP, 0, "DTLSv1_listen() returned %d\n", listenStatus );
			SSL_shutdown( ssl );
			SSL_free( ssl );
			continue;
		}
		log( LL_INFO, 0, "New connection offered %s:%d\n", inet_ntoa( clientAddr.sin_addr ), ntohs( clientAddr.sin_port ) );

		// Accept the connection
		log( LL_DUMP, 0, "Before SSL_accept\n" );
		status = SSL_accept( ssl );
		log( LL_DUMP, 0, "After SSL_accept\n" );
		if (status != 1) {
			log( LL_WARNING, 0, "SSL_accept() returned %d\n", status );
			SSL_shutdown( ssl );
			SSL_free( ssl );
			continue;
		}
		log( LL_DUMP, 0, "Connection is accepted\n" );

		// Start read/reply cycle
		while (1)
		{
			// Receive message
			const int buf_size = 4096;
			char buffer[buf_size];
			int len = SSL_read( ssl, buffer, sizeof( buffer ) );
			if (len > 0) {
				buffer[len] = 0;
				log( LL_INFO, (int)listeningSocket, "%s:%d IN : %s\n", inet_ntoa( clientAddr.sin_addr ), ntohs( clientAddr.sin_port ), buffer );
			}
			else
			{
				printf( "Connection closed, SSL_read() returned %d\n", len );
				log( LL_INFO, (int)listeningSocket, "%s:%d Connection closed\n", inet_ntoa( clientAddr.sin_addr ), ntohs( clientAddr.sin_port ) );
				break;
			}

			// Send echo responce
			char echoBuffer[buf_size];
			strncpy( echoBuffer, "Server Echo: ", buf_size );
			strncat( echoBuffer, buffer, buf_size );
			len = SSL_write( ssl, echoBuffer, strlen( echoBuffer ) );
			if (strlen( echoBuffer ) != len) {
				log( LL_WARNING, (int)listeningSocket, "Failed to write the entire buffer to %s:%d. Connection closed\n",
					inet_ntoa( clientAddr.sin_addr ), ntohs( clientAddr.sin_port ) );
				break;
			}
			log( LL_INFO, (int)listeningSocket, "%s:%d OUT: %s\n", inet_ntoa( clientAddr.sin_addr ), ntohs( clientAddr.sin_port ), echoBuffer );
		}
		// Close  SSL session
		SSL_shutdown( ssl );
		SSL_free( ssl );

	} // Main cycle
}
