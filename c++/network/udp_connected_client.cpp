/*
	Simple UDP client. Send message and waits for responce
	Uses connected UDP socket
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

#define DEFAULT_LOCAL_PORT 32001

// extern from common.cpp
void printAddrInfoItem( struct addrinfo *pAddrinfo );

int main( int argc, char *argv[] )
{
	struct addrinfo hints, *serverInfo, *ptr;
	int sockfd;
	int status;
	int localPort = DEFAULT_LOCAL_PORT;

	if (argc < 4) {
		fprintf( stderr, "usage: %s <hostname> <port> <message> [<local_port>]\n", argv[0] );
		exit( 1 );
	}
	if (argc == 5) localPort = atoi(argv[4]);

	// Prepare client address that will connect to server using UDP IPv4
	memset( &hints, 0, sizeof hints );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	if (( status = getaddrinfo( argv[1], argv[2], &hints, &serverInfo ) ) != 0) {
		fprintf( stderr, "getaddrinfo() error: %s\n", gai_strerror( status ) );
		return -1;
	}

	// loop through all the results and make a socket
	for (ptr = serverInfo; ptr != nullptr; ptr = ptr->ai_next) {
		printf( "Try addrinfo:\n" );
		printAddrInfoItem( ptr );
		if (( sockfd = socket( ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol ) ) == -1) {
			fprintf( stderr, "socket() error: %s\n", strerror( errno ) );
			continue;
		}
		break;
	}

	if (ptr == nullptr) {
		fprintf( stderr, "Could not open socket with provided addrinfos\n" );
		return -1;
	}

	// bind socket to local client's port
	struct sockaddr_in localAddr;
	memset( &localAddr, 0, sizeof( sockaddr_in ) );
	localAddr.sin_family = ptr->ai_addr->sa_family;
	localAddr.sin_port = htons( localPort );

	if (bind( sockfd, ( struct sockaddr * )&localAddr, sizeof( localAddr ) ) == -1)
	{
		fprintf( stderr, "bind() error: %s\n", strerror( errno ) );
		return -1;
	}

	// connect socket to remote server
	if (connect( sockfd, ptr->ai_addr, ptr->ai_addrlen ) != 0)
	{
		fprintf( stderr, "connect() error: %s\n", strerror( errno ) );
		return -1;
	}

	// Send message to remote server
	int msgLen = strlen( argv[3] );
	int sentBytes = send( sockfd, argv[3], msgLen, 0 );
	if (sentBytes != msgLen)
	{
		fprintf( stderr, "send() error : returned %d\n", sentBytes );
	}
	else
	{
		printf( "OUT[%d]: %s\n", sentBytes, argv[3] );
	}

	freeaddrinfo( serverInfo );
	close( sockfd );
	return 0;
}
