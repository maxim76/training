/*
	Tests getsockname() function

	int getsockname(int sockfd, struct sockaddr *addrsocklen_t *" addrlen );

	Returns the current address to which the socket sockfd is bound, in the buffer pointed to by addr. 
	The addrlen argument should be initialized to indicate the amount of space (in bytes) pointed to by addr. 
	On return it contains the actual size of the socket address.

	Full getaddrinfo() documentation is at: https://linux.die.net/man/3/getsockname

	Used structs:
	
	struct sockaddr {
		unsigned short sa_family;		// address family, AF_xxx. Usually AF_INET (IPv4) or AF_INET6 (IPv6)
		char sa_data[14];				// 14 bytes of protocol address, depends on "sa_family" value
	};

	// Keeps IP address and port for IPv4
	struct sockaddr_in {
		short int sin_family;			// Address family, AF_INET
		unsigned short int sin_port;	// Port number in network byte order, use htons()/ntohs()
		struct in_addr sin_addr;		// Internet address
		unsigned char sin_zero[8];		// Same size as struct sockaddr
	};

	// IP address, IPv4 only. (a structure for historical reasons)
	struct in_addr {
		uint32_t s_addr;				// IPv4 address
	};

	// Keeps IP address and port for IPv6
	struct sockaddr_in6 {
		u_int16_t sin6_family;			// address family, AF_INET6
		u_int16_t sin6_port;			// port number, Network Byte Order
		u_int32_t sin6_flowinfo;		// IPv6 flow information
		struct in6_addr sin6_addr;		// IPv6 address
		u_int32_t sin6_scope_id;		// Scope ID
	};

	// IP address, IPv6 only. (a structure for historical reasons)
	struct in6_addr {
		unsigned char s6_addr[16];		// IPv6 address
	};

	// sockaddr_storage is struct that big enough to fit both sockaddr_in and sockaddr_in6 and used when 
	// it's not known in advance which one IPv4 or IPv6 will be filled in
	struct sockaddr_storage {
		sa_family_t ss_family;			// address family
		char __ss_pad1[_SS_PAD1SIZE];	// padding
		int64_t __ss_align;				// padding
		char __ss_pad2[_SS_PAD2SIZE];	// padding
	};
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
#include <unistd.h>

#define RANDOM_PORT		"0"

void printAddrInfo( struct addrinfo *pAddrinfo );	// extern from common.cpp
void printIPPort( struct sockaddr *sa );			// extern from common.cpp

int main()
{
	struct addrinfo hints;			// criteria for selecting the socket address
	struct addrinfo *res;			// returned result - linked list of addrinfo stucts with addresses that matches node and service
	int status;
	int listeningSocket;

	printf( "\nPreparing server address that will listen for incoming connection on local address, random port using UDP IPv4...\t" );
	// AI_PASSIVE flag instructs to bind to the IP of the local host.	If you want to bind to a specific local IP address, dont use the AI_PASSIVE and provide an IP address in the first argument to getaddrinfo().
	memset( &hints, 0, sizeof( struct addrinfo ) );
	hints.ai_family		= AF_INET;		// IPv4
	hints.ai_socktype	= SOCK_DGRAM;	// UDP socket
	hints.ai_flags		= AI_PASSIVE;	// For wildcard IP address. If param "node" in getaddrinfo() is not NULL, AI_PASSIVE is ignored

	if (( status = getaddrinfo( NULL, RANDOM_PORT, &hints, &res ) ) != 0)
	{
		printf( "getaddrinfo() error: %s\n", gai_strerror( status ) );
		return -1;
	}
	printf( "Done\n" );
	if (res == nullptr)
	{
		printf( "getaddrinfo() returned empty address list\n" );
		return -1;

	}
	/*
		Or fill sockaddr_in manually :
		struct sockaddr_in serverAddr;
		memset( (char *)&serverAddr, 0, sizeof(sockaddr_in) );
		serverAddr.sin_family = AF_INET;	// IPV4
		serverAddr.sin_port = htons( RANDOM_PORT );
	*/

	printf( "\nPrepared addrinfo structure:\n" );
	printAddrInfo( res );

	printf( "\nOpening socket...:\t" );
	if (( listeningSocket = socket( res->ai_family, res->ai_socktype, res->ai_protocol ) ) == -1)
	{
		printf( "socket() error: %s\n", strerror( errno ) );
		return -1;
	}
	printf( "Done\n" );

	printf( "\nBinding socket...:\t" );
	if (bind( listeningSocket, res->ai_addr, res->ai_addrlen ) == -1)
	{
		printf( "bind() error: %s\n", strerror( errno ) );
		return -1;
	}
	printf( "Done\n" );
	freeaddrinfo( res );

	printf( "\nGetting socket address...:\t" );
	struct sockaddr_in sin;
	socklen_t addrlen = sizeof( sin );
	if (getsockname( listeningSocket, ( struct sockaddr * )&sin, &addrlen ) == -1)
	{
		printf( "getsockname() error: %s\n", strerror( errno ) );
		return -1;
	}
	printf( "Done\n" );

	printf( "\nSocket name is:\n" );
	printIPPort( ( struct sockaddr * )&sin );
	printf( "\n" );

	close( listeningSocket );
}
