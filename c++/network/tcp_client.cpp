/*
	Simple TCP client.
	Connects to TCP server and reads data from socket
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXDATASIZE 100 // max number of bytes we can get at once

void printIPPort( struct sockaddr *sa ); // extern from common.cpp


int main( int argc, char *argv[] )
{
	int sockfd;
	struct addrinfo hints, *servinfo, *ptr;
	int status;
	int recvBytes;
	char buf[MAXDATASIZE];

	if (argc != 3)
	{
		fprintf( stderr, "Usage: %s <server_ip> <server_port>\n", argv[0] );
		return -1;
	}

	// Find suitable addrinfo and create socket
	memset( &hints, 0, sizeof hints );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if (( status = getaddrinfo( argv[1], argv[2], &hints, &servinfo ) ) != 0) {
		fprintf( stderr, "getaddrinfo() error: %s\n", gai_strerror( status ) );
		return -1;
	}

	for (ptr = servinfo; ptr != NULL; ptr = ptr->ai_next) {
		if (( sockfd = socket( ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol ) ) == -1) {
			fprintf( stderr, "socket() error: %s\n", strerror( errno ) );
			continue;
		}

		if (connect( sockfd, ptr->ai_addr, ptr->ai_addrlen ) == -1) {
			close( sockfd );
			fprintf( stderr, "connect() error: %s\n", strerror( errno ) );
			continue;
		}
		break;
	}

	if (ptr == NULL) {
		fprintf( stderr, "Failed to create socket/connect\n" );
		return -1;
	}

	printf( "Connected to: " ); printIPPort( ( struct sockaddr * )&ptr ); printf( "\n" );
	freeaddrinfo( servinfo );

	if (( recvBytes = recv( sockfd, buf, MAXDATASIZE - 1, 0 ) ) == -1) {
		fprintf( stderr, "recv() error: %s\n", strerror( errno ) );
		return -1;
	}
	buf[recvBytes] = '\0';
	printf( "IN [%d]: %s\n", recvBytes, buf );

	close( sockfd );
	return 0;
}
