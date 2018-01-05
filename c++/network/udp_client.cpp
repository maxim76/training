/*
	Simple UDP client.
	Sends text message to UDP server
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


int main( int argc, char *argv[] )
{
	int sockfd;
	struct addrinfo hints, *aiList, *aiItem;
	int status;
	int numbytes;

	if (argc != 4)
	{
		fprintf( stderr, "Usage: %s <server_ip> <server_port> <message>\n", argv[0] );
		return -1;
	}

	// Find suitable addrinfo and create socket
	memset( &hints, 0, sizeof hints );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	if (( status = getaddrinfo( argv[1], argv[2], &hints, &aiList ) ) != 0) {
		fprintf( stderr, "getaddrinfo() error: %s\n", gai_strerror( status ) );
		return -1;
	}

	for (aiItem = aiList; aiItem != NULL; aiItem = aiItem->ai_next) {
		if (( sockfd = socket( aiItem->ai_family, aiItem->ai_socktype, aiItem->ai_protocol ) ) == -1) {
			fprintf( stderr, "socket() error: %s\n", strerror( errno ) );
			continue;
		}
		break;
	}

	if (aiItem == nullptr) {
		fprintf( stderr, "Failed to create socket\n" );
		return -1;
	}

	// Send message
	if (( numbytes = sendto( sockfd, argv[3], strlen( argv[3] ), 0,
		aiItem->ai_addr, aiItem->ai_addrlen ) ) == -1) {
		fprintf( stderr, "sendto() error: %s\n", strerror( errno ) );
		return -1;
	}

	printf( "Sent %d bytes to %s:%s\n", numbytes, argv[1], argv[2] );

	// Free
	freeaddrinfo( aiList );
	close( sockfd );
	return 0;
}
