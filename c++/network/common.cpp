/*
	Common network related funcs used by examples
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>


void printAddrInfoItem( struct addrinfo *ptr )
{
	void *addr;
	int port;
	char ipver[64];
	char ipstr[128];
	char proto[16];
	char socktype[16];

	if (ptr->ai_family == AF_INET) { // IPv4
		struct sockaddr_in *ipv4 = ( struct sockaddr_in * )ptr->ai_addr;
		addr = &( ipv4->sin_addr );
		port = ntohs( ipv4->sin_port );
		strcpy( ipver, "IPv4" );
	}
	else { // IPv6
		struct sockaddr_in6 *ipv6 = ( struct sockaddr_in6 * )ptr->ai_addr;
		addr = &( ipv6->sin6_addr );
		port = ntohs( ipv6->sin6_port );
		strcpy( ipver, "IPv6" );
	}

	inet_ntop( ptr->ai_family, addr, ipstr, sizeof ipstr );
	switch (ptr->ai_protocol)
	{
	case 6:
		strcpy( proto, "TCP" );
		break;
	case 17:
		strcpy( proto, "UDP" );
		break;
	default:
		sprintf( proto, "%d", ptr->ai_protocol );
	}
	switch (ptr->ai_socktype)
	{
	case SOCK_STREAM:
		strcpy( socktype, "STREAM" );
		break;
	case SOCK_DGRAM:
		strcpy( socktype, "DGRAM" );
		break;
	case SOCK_RAW:
		strcpy( socktype, "RAW" );
		break;
	default:
		sprintf( socktype, "%d", ptr->ai_socktype );
	}

	printf( "Flags: %2d  Family: %4s  Type: %-8s  Proto:  %-8s  ", ptr->ai_flags, ipver, socktype, proto );
	printf( "IPaddr: %-40s Port: %d\n", ipstr, port );

}

void printAddrInfo( struct addrinfo *pAddrinfo )
{
	struct addrinfo *ptr;
	for (ptr = pAddrinfo; ptr != NULL; ptr = ptr->ai_next) {
		printAddrInfoItem( ptr );
	}

}

void printIPPort( struct sockaddr *sa )
{
	void *addr;
	int port;
	char ipstr[128];

	if (sa->sa_family == AF_INET)
	{
		struct sockaddr_in *ipv4 = ( struct sockaddr_in * )sa;
		addr = &( ipv4->sin_addr );
		port = ntohs( ipv4->sin_port );
	}
	else
	{
		struct sockaddr_in6 *ipv6 = ( struct sockaddr_in6 * )&sa;
		addr = &( ipv6->sin6_addr );
		port = ntohs( ipv6->sin6_port );
	}
	inet_ntop( sa->sa_family, addr, ipstr, sizeof ipstr );
	printf( "%s:%d", ipstr, port );
}
