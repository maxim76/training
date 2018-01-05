/*
	Tests connect() function.
	Connects to www.example.com:80

	connect() documentation:	https://linux.die.net/man/3/connect
	shutdown() documentation:	https://linux.die.net/man/2/shutdown
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>


void printAddrInfo( struct addrinfo *pAddrinfo );	// extern from common.cpp

int main()
{
	struct addrinfo hints, *res;
	int sockfd;
	int status;

	// Use getaddrinfo() to prepare remote server address
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if (( status = getaddrinfo("www.example.com", "80", &hints, &res )) != 0) 
	{
		printf("getaddrinfo() error: %s\n", gai_strerror(status));
		return -1;
	}
	printAddrInfo( res );

	// Use the first result as the address to connect to
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);


	// bind() can be missed there. connect() will bind socket automatically to a random free port on local machime
	struct sockaddr_in my_addr;
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons( 1234 );		// short, network byte order
	//my_addr.sin_addr.s_addr = inet_addr( "10.12.110.57" );
	memset( my_addr.sin_zero, '\0', sizeof my_addr.sin_zero );

	// bind() is used. Local IP address and port will be used as stated in addr struct
	// without bind random free port on local machime will be used as client port
	bind( sockfd, ( struct sockaddr * )&my_addr, sizeof my_addr );
	if (( status = connect(sockfd, res->ai_addr, res->ai_addrlen)) != 0)
	{
		printf( "connect() error: %s\n", strerror( errno ) );
		return -1;
	}
	printf( "Connected successfully\n" );
	if (shutdown( sockfd, SHUT_RDWR ) == -1)
	{
		printf( "shutdown() error: %s\n", strerror( errno ) );
		return -1;
	}
	printf( "Disconnected successfully\n" );
	close( sockfd );
}
