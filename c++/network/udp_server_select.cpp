/*
	UDP server. Waits for incoming connection. After getting connection, read message and echoes it back.
	Uses connected UDP socket with client and select() func to run correct socket handler

	Testing: use 'udp_send' tool with 'local port' parameter.
	udp_server_select associates remote ports with different sockets.

	Example:
	1. Start ./udp_server_select 127.0.0.1 1234

	2. Send a few packets from udp_send using different local ports:
		./bin/linux/udp_send -d 127.0.0.1:1234 -m Hello -l 32000
		./bin/linux/udp_send -d 127.0.0.1:1234 -m Hello -l 32000
		./bin/linux/udp_send -d 127.0.0.1:1234 -m Hello -l 32001
		./bin/linux/udp_send -d 127.0.0.1:1234 -m Hello -l 32001
		./bin/linux/udp_send -d 127.0.0.1:1234 -m Hello -l 32000
		./bin/linux/udp_send -d 127.0.0.1:1234 -m Hello -l 32001

	3. udp_server_select receives:
		INFO: Starting Server On 127.0.0.1:1234
		Waiting for incoming packets...
		New connection from 127.0.0.1:32000
		[4] New socket created
		IN [4]: Hello
		IN [4]: Hello
		New connection from 127.0.0.1:32001
		[5] New socket created
		IN [5]: Hello
		IN [5]: Hello
		IN [4]: Hello
		IN [5]: Hello

*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#endif


#ifdef WIN32
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996) 
#endif

#define MAXBUFLEN 1024
#define CONNECTIONS_BACKLOG	16


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


int main( int argc, char *argv[] )
{
	if (argc != 3)
	{
		fprintf( stderr, "Usage: %s <server_ip> <server_port>\n", argv[0] );
		return -1;
	}

	int serverPort = atoi( argv[2] );

	fd_set allSockets;
	fd_set readSockets;
	int maxSocket;

#ifdef WIN32
	// Initialize socket library
	WSADATA wsaData;
	if (WSAStartup( MAKEWORD( 1, 1 ), &wsaData ) != 0) {
		fprintf( stderr, "WSAStartup() failed\n" );
		return -1;
	}
	typedef int socklen_t;
#endif

	// Open listening socket and bind to server:port
	struct sockaddr_in serverAddr;
	memset( (void *)&serverAddr, 0, sizeof( struct sockaddr_in ) );
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons( serverPort );
	serverAddr.sin_addr.s_addr = inet_addr( argv[1] );

	printf( "INFO: Starting Server On %s:%d\n", inet_ntoa( serverAddr.sin_addr ), ntohs( serverAddr.sin_port ) );

	int listeningSocket;
	if (( listeningSocket = socket( PF_INET, SOCK_DGRAM, 0 ) ) == -1)
	{
		fprintf( stderr, "socket() error: %s\n", strerror( errno ) );
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

	if (( bind( listeningSocket, ( struct sockaddr * )&serverAddr, sizeof( struct sockaddr_in ) ) ) == -1)
	{
		fprintf( stderr, "bind() error: %s\n", strerror( errno ) );
		return -1;
	}

	FD_ZERO( &allSockets );
	FD_ZERO( &readSockets );
	FD_SET( listeningSocket, &allSockets );
	maxSocket = listeningSocket;
	int clientSocket;
	struct sockaddr_in clientAddr;
	int clientAddrLen = sizeof( clientAddr );
	int recvBytes;
	char buf[MAXBUFLEN + 1];

	printf( "\nWaiting for incoming packets...\n" );
	while (true)
	{
		// Get list of sockets with pending data
		readSockets = allSockets;
		if (( select( maxSocket + 1, &readSockets, NULL, NULL, NULL ) ) == -1)
		{
			fprintf( stderr, "select() error: %s\n", strerror( errno ) );
			return -1;
		}
		// Process all sockets one by one
		for (int sock = 0; sock <= maxSocket; sock++)
		{
			if (FD_ISSET( sock, &readSockets ))
			{
				if (sock == listeningSocket)	// data on listening socket. It must be a new connection request
				{
					// Read data from listenSocket first to obtain remote address
					if (( recvBytes = recvfrom( listeningSocket, buf, MAXBUFLEN, 0, ( struct sockaddr * )&clientAddr, ( socklen_t *)&clientAddrLen ) ) == -1)
					{
						printf( "recvfrom() error: %s\n", strerror( errno ) );
						continue;
					}
					printf("New connection from %s:%d\n", inet_ntoa( clientAddr.sin_addr ), ntohs( clientAddr.sin_port ) );

					// Create separate socket for this client
					clientSocket = socket( PF_INET, SOCK_DGRAM, 0 );

					// Allow socket to reuse the same address/port
					if (setSocketOption<int>( clientSocket, SOL_SOCKET, SO_REUSEADDR, 1 ) == -1)
					{
						fprintf( stderr, "setsockopt( SO_REUSEADDR ) error: %s\n", strerror( errno ) );
						return -1;
					}
#ifndef WIN32
					if (setSocketOption<int>( clientSocket, SOL_SOCKET, SO_REUSEPORT, 1 ) == -1)
					{
						fprintf( stderr, "setsockopt( SO_REUSEPORT ) error: %s\n", strerror( errno ) );
						return -1;
					}
#endif

					// Bind to client socket to the same local server port
					if (( bind( clientSocket, ( struct sockaddr * )&serverAddr, sizeof( struct sockaddr_in ) ) ) == -1)
					{
						printf( "client socket bind() error: %s\n", strerror( errno ) );
						//close( clientSocket );
						continue;
					}


					// And make socket connected
					if (connect( clientSocket, ( struct sockaddr * )&clientAddr, clientAddrLen ) == -1)
					{
						printf( "client socket connect() error: %s\n", strerror( errno ) );
						//close( clientSocket );
						continue;
					}
					printf( "[%d] New socket created\n", (int)clientSocket );
					buf[recvBytes] = 0;
					printf( "IN [%d]: %s\n", (int)clientSocket, buf );

					FD_SET( clientSocket, &allSockets );
					if (clientSocket > maxSocket) maxSocket = clientSocket;
				}
				else // data on connected socket
				{
					if (( recvBytes = recv( sock, buf, MAXBUFLEN, 0) ) == -1)
					{
						printf( "recvfrom() error: %s\n", strerror( errno ) );
						continue;
					}
					buf[recvBytes] = 0;
					printf( "IN [%d]: %s\n", (int)sock, buf );
				}
			}
		}
	}
}
