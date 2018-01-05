/*
	Tests listen() function
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MYPORT	"1234"
#define BACKLOG	10

// extern from common.cpp
void printAddrInfoItem( struct addrinfo *pAddrinfo );
void printIPPort( struct sockaddr *sa );

int main()
{
	struct addrinfo hints, *res;
	int listenSocket, clientSocket;
	struct sockaddr_storage clientAddr;
	socklen_t addrSize;

	memset( &hints, 0, sizeof hints );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo( NULL, MYPORT, &hints, &res );

	if (res != NULL)
	{
		printf( "Suitable local addrinfo is:\n" );
		printAddrInfoItem( res );
	}
	else
	{
		printf( "No suitable interface found\n" );
		return -1;
	}

	if (( listenSocket = socket( res->ai_family, res->ai_socktype, res->ai_protocol ) ) == -1)
	{
		printf( "socket() error: %s\n", strerror( errno ) );
	}

	if (bind( listenSocket, res->ai_addr, res->ai_addrlen ) == -1)
	{
		printf( "bind() error: %s\n", strerror( errno ) );
		return -1;
	}

	if (listen( listenSocket, BACKLOG ) == -1)
	{
		printf( "listen() error: %s\n", strerror( errno ) );
		return -1;
	}

	struct sockaddr_in *serverAddr = ( struct sockaddr_in * )res->ai_addr;
	printf( "Listening socket opened. Try to connect to %s:%d\n", inet_ntoa( serverAddr->sin_addr ), ntohs( serverAddr->sin_port ) );

	addrSize = sizeof( clientAddr );
	if ((clientSocket = accept( listenSocket, ( struct sockaddr * )&clientAddr, &addrSize )) == -1)
	{
		printf( "accept() error: %s\n", strerror( errno ) );
		return -1;
	}

	printf( "Connection is accepted from " );	printIPPort( ( struct sockaddr * )&clientAddr );	printf( "\n" );

	// Try same with getpeername()
	struct sockaddr_storage clientAddr2;
	socklen_t addrSize2 = sizeof( struct sockaddr_storage );
	if (getpeername( clientSocket, ( struct sockaddr * )&clientAddr2, &addrSize2 ) == -1)
	{
		printf( "getpeername() error: %s\n", strerror( errno ) );
		return -1;
	}
	printf( "Peer address returned by getpeername() is " );	printIPPort( ( struct sockaddr * )&clientAddr2 );	printf( "\n" );
}
