/*
	Tests getaddrinfo() function

	int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);

	Params:
	*node		- Node to connect to. Can be numerical IP-address, alfanumerical hostname or NULL.
				When NULL (and AI_PASSIVE is on in hints) addr will be suitable for accept()
				I.e. returned address will contain wildcard : INADDR_ANY for IPv4 addresses, IN6ADDR_ANY_INIT for IPv6
	*service	- sets the port in each returned struct. Can be NULL, but node and service cannot be NULL both
	&hints		- specifies criteria that limit the set of socket addresses returned by getaddrinfo(). 
				Fields that can be set as filters are: ai_flags, ai_family, ai_socktype, ai_protocol. All others field must be 0/NULL.
	**res		- returned linked list of allocated structures

    Given node and service, which identify an Internet host and a service, getaddrinfo() returns one or more addrinfo 
	structures, each of which contains an Internet address that can be specified in a call to bind(2) or connect(2).
	Full getaddrinfo() documentation is at: https://linux.die.net/man/3/getaddrinfo

	The addrinfo structure used by getaddrinfo() contains the following fields:

	struct addrinfo {
	    int              ai_flags;		// AI_PASSIVE, AI_CANONNAME, etc.
	    int              ai_family;		// AF_INET, AF_INET6, AF_UNSPEC
	    int              ai_socktype;	// SOCK_STREAM, SOCK_DGRAM. 0 means any type
	    int              ai_protocol;	// specifies the protocol for the returned socket addresses. Use 0 for "any"
	    socklen_t        ai_addrlen;	// size of ai_addr in bytes
	    struct sockaddr *ai_addr;		// struct sockaddr_in or sockaddr_in6, depends on address family
	    char            *ai_canonname;	// full canonical hostname
	    struct addrinfo *ai_next;		// linked list, next node
	};

	Possible flags:
		AI_PASSIVE - is set and node is NULL, then the returned socket addresses will be suitable for bind(2)ing a socket that will accept(2) connections. 
					The returned socket address will contain the "wildcard address" (INADDR_ANY for IPv4 addresses, IN6ADDR_ANY_INIT for IPv6 address). 
					If node is not NULL, then the AI_PASSIVE flag is ignored.
					If the AI_PASSIVE flag is not set, then the returned socket addresses will be suitable for use with connect(2), sendto(2), or sendmsg(2). 
					If node is NULL and flag is not set, then the network address will be set to the loopback interface address (INADDR_LOOPBACK for IPv4 addresses, IN6ADDR_LOOPBACK_INIT for IPv6 address); 
					this is used by applications that intend to communicate with peers running on the same host.
					Technically flag means that it will be server connection when flag is set, or client when it is not set.
		AI_ADDRCONFIG - return any IPv4 addresses only if system has at least one IPv4 address configured and same for IPv6.
					The loopback address is not considered for this case as valid as a configured address.

	Other underlying structs:
	
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>


void printAddrInfo( struct addrinfo *pAddrinfo );	// extern from common.cpp

int main()
{
	struct addrinfo hints;			// criteria for selecting the socket address
	struct addrinfo *result;		// returned result - linked list of addrinfo stucts with addresses that matches node and service
	int status;

	// TEST 1
	printf( "\nTest1 Prepare client address that will connect to server www.example.com:8080 using TCP IPv4\n" );
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;		// Allow IPv4 only
	hints.ai_socktype = SOCK_STREAM; // TCP socket
	hints.ai_flags = 0;			    // No special flag
	hints.ai_protocol = 0;          // Any protocol
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	if (( status = getaddrinfo("www.example.com", "8080", &hints, &result )) != 0) 
	{
		printf("getaddrinfo() error: %s\n", gai_strerror(status));
		exit(1);
	}

	printAddrInfo( result );
	freeaddrinfo(result);

	// TEST 2
	printf( "\nTest2 Prepare client address that will connect to server www.example.com:8080 using any protocol and any address family\n" );
	memset( &hints, 0, sizeof( struct addrinfo ) );
	hints.ai_family = AF_UNSPEC;		// IPv4 and IPv6
	hints.ai_socktype = 0;				// any type of socket
	// With such empty hints settings it's also possible not to specify hints at all (use NULL as hints parameter, and getaddrinfo() result will be the same)
	if (( status = getaddrinfo( "www.example.com", "8080", &hints, &result ) ) != 0)
	{
		printf( "getaddrinfo() error: %s\n", gai_strerror( status ) );
		exit( 1 );
	}

	printAddrInfo( result );
	freeaddrinfo( result );

	// TEST 3
	printf( "\nTest3 Prepare client address that will connect to localhost:1234 using UDP IPv4\n" );
	memset( &hints, 0, sizeof( struct addrinfo ) );
	hints.ai_family = AF_INET;			// IPv4 and IPv6
	hints.ai_socktype = SOCK_DGRAM;		// UDP socket
	if (( status = getaddrinfo( NULL, "1234", &hints, &result ) ) != 0)
	{
		printf( "getaddrinfo() error: %s\n", gai_strerror( status ) );
		exit( 1 );
	}

	printAddrInfo( result );
	freeaddrinfo( result );


	// TEST 4
	printf( "\nTest4. Prepare server address that will listen for incoming connection on local address, port 32000 using UDP IPv4\n" );
	// AI_PASSIVE flag instructs to bind to the IP of the local host.	If you want to bind to a specific local IP address, dont use the AI_PASSIVE and provide an IP address in the first argument to getaddrinfo().
	memset( &hints, 0, sizeof( struct addrinfo ) );
	hints.ai_family = AF_INET;			// IPv4 and IPv6
	hints.ai_socktype = SOCK_DGRAM;		// UDP socket
	hints.ai_flags = AI_PASSIVE;		// For wildcard IP address. If "node" is not NULL, AI_PASSIVE is ignored

	if (( status = getaddrinfo( NULL, "32000", &hints, &result ) ) != 0)
	{
		printf( "getaddrinfo() error: %s\n", gai_strerror( status ) );
		exit( 1 );
	}

	printAddrInfo( result );
	freeaddrinfo( result );


}
