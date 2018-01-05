/*
	UDP server. Waits for incoming connection. After getting connection, read message and echoes it back.
	Uses connected UDP socket with client
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAXBUFLEN 1024

// extern from common.cpp
void printAddrInfoItem( struct addrinfo *pAddrinfo );
void printIPPort( struct sockaddr *sa );

int main( int argc, char *argv[] )
{
	if (argc != 3)
	{
		fprintf( stderr, "Usage: %s <server_ip> <server_port>\n", argv[0] );
		return -1;
	}

	struct addrinfo hints, *serverInfo, *ptr;
	struct sockaddr *serverAddr;
	int serverAddrLen;
	int listenSocket;
	int status;

	// Prepare server addressinfo for listening
	memset( &hints, 0, sizeof hints );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	if (( status = getaddrinfo( NULL, argv[2], &hints, &serverInfo ) ) != 0) {
		fprintf( stderr, "getaddrinfo() error : %s\n", gai_strerror( status ) );
		return -1;
	}

	// Loop through all the results and bind to the first we can
	for (ptr = serverInfo; ptr != NULL; ptr = ptr->ai_next) {
		printf( "Try addrinfo:\n" );
		printAddrInfoItem( ptr );
		if (( listenSocket = socket( ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol ) ) == -1) {
			fprintf( stderr, "socket() error: %s\n", strerror( errno ) );
			continue;
		}
		if (bind( listenSocket, ptr->ai_addr, ptr->ai_addrlen ) == -1) {
			close( listenSocket );
			fprintf( stderr, "bind() error: %s\n", strerror( errno ) );
			continue;
		}
		break;
	}

	if (ptr == nullptr) {
		printf( "Could not open/bind socket\n" );
		return -1;
	}
	serverAddr = ( struct sockaddr * )ptr->ai_addr;
	serverAddrLen = ptr->ai_addrlen;
	freeaddrinfo( serverInfo );

	while (1)
	{
		struct sockaddr_storage clientAddr;
		socklen_t clientAddrLen;
		int recvBytes;
		int sentBytes;
		char buf[MAXBUFLEN + 1];

		printf( "\nWaiting for incoming packets...\n" );
		clientAddrLen = sizeof( clientAddr );
		if (( recvBytes = recvfrom( listenSocket, buf, MAXBUFLEN, 0, ( struct sockaddr * )&clientAddr, &clientAddrLen ) ) == -1)
		{
			fprintf( stderr, "recvfrom() error: %s\n", strerror( errno ) );
			continue;
		}
		if (recvBytes == 0)
		{
			fprintf( stderr, "recvfrom() error: socket is closed\n" );
			continue;
		}
		else
		{
			printf( "Got packet from " ); printIPPort( ( struct sockaddr * )&clientAddr );	printf( " to " ); printIPPort( serverAddr ); printf( "\n" );
			buf[recvBytes] = 0;
			printf( "IN [%d]: %s\n", recvBytes, buf );

			// Connect socket to remote client
			int clientSocket;
			struct sockaddr_storage serverInstanceAddr;
			socklen_t serverInstanceAddrLen = sizeof( serverInstanceAddr );

			if (( clientSocket = socket( AF_INET, SOCK_DGRAM, 0 ) ) == -1) {
				fprintf( stderr, "client socket creation socket() error: %s\n", strerror( errno ) );
				continue;
			}
			if (connect( clientSocket, ( struct sockaddr * )&clientAddr, clientAddrLen ) == -1)
			{
				fprintf( stderr, "client socket connect() error: %s\n", strerror( errno ) );
				close( clientSocket );
				continue;
			}
			if (( getsockname( clientSocket, ( struct sockaddr * )&serverInstanceAddr, &serverInstanceAddrLen ) ))
			{
				fprintf( stderr, "client socket getsockname() error: %s\n", strerror( errno ) );
				close( clientSocket );
				continue;
			}
			
			// Send echo responce
			char echoBuffer[MAXBUFLEN + 1];
			strncpy( echoBuffer, "Server Echo: ", MAXBUFLEN );
			strncat( echoBuffer, buf, recvBytes );
			printf( "Sending echo responce from " ); printIPPort( ( struct sockaddr * )&serverInstanceAddr ); printf( " to " ); printIPPort( ( struct sockaddr * )&clientAddr ); printf( "\n" );
			if (( sentBytes = send( clientSocket, echoBuffer, strlen( echoBuffer ), 0 ) ) == -1)
			{
				fprintf( stderr, "send() error: %s\n", strerror( errno ) );
				continue;
			}
			else
			{
				printf( "OUT[%d]: %s\n", sentBytes, echoBuffer );
			}
		}
	}
}
