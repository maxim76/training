/*
	Tests SO_REUSEADDR socket option

	Opens main socket and binds it to port. Then sets SO_REUSEADDR option and
	opens/binds additional sockets to the same port.

	Before the first binding, tries to bind in exclusive mode (without SO_REUSEADDR option) to check that no other application instance already uses the same port in shared mode.

	Socket options is listed here: http://man7.org/linux/man-pages/man7/socket.7.html
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>


void printIPPort( struct sockaddr *sa );			// extern from common.cpp

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


int allowReuseAddress( int socket )
{
	if (setSocketOption<int>( socket, SOL_SOCKET, SO_REUSEADDR, 1 ) == -1)
	{
		fprintf( stderr, "setsockopt( SO_REUSEADDR ) error: %s\n", strerror( errno ) );
		return -1;
	}
#ifndef WIN32
	if (setSocketOption<int>( socket, SOL_SOCKET, SO_REUSEPORT, 1 ) == -1)
	{
		fprintf( stderr, "setsockopt( SO_REUSEPORT ) error: %s\n", strerror( errno ) );
		return -1;
	}
#endif
	return 0;
}


int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		fprintf( stderr, "Usage: %s <server_ip> <server_port>\n", argv[0] );
		return -1;
	}

	int serverPort = atoi( argv[2] );

	int sockMaster = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if (sockMaster == -1)
	{
		fprintf( stderr, "socket() failed with error: %s\n", strerror( errno ) );
		return -1;
	}

	/*
	if (setSocketOption<int>( sockMaster, SOL_SOCKET, SO_REUSEADDR, 1 ) == -1)
	{
		fprintf( stderr, "Master socket setsockopt( SO_REUSEADDR ) error: %s\n", strerror( errno ) );
		return -1;
	}
	*/

	struct sockaddr_in serverAddr;
	memset( (void *)&serverAddr, 0, sizeof( struct sockaddr_in ) );
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons( serverPort );
	serverAddr.sin_addr.s_addr = inet_addr( argv[1] );

	printf( "Trying bind to address %s:%d\n", argv[1], serverPort );
	if (( bind( sockMaster, ( struct sockaddr * )&serverAddr, sizeof( struct sockaddr_in ) ) ) == -1)
	{
		printf( "Failed\n" );
		fprintf( stderr, "Master socket bind() error: %s\n", strerror( errno ) );

		printf( "Trying bind to random port\n" );
		serverAddr.sin_port = 0;
		if (( bind( sockMaster, ( struct sockaddr * )&serverAddr, sizeof( struct sockaddr_in ) ) ) == -1)
		{
			printf( "Failed\n" );
			fprintf( stderr, "Master socket bind() error: %s\n", strerror( errno ) );
			return -1;
		}
	}

	// Check final master socket address
	struct sockaddr_in sinMaster;
	socklen_t addrlen = sizeof( struct sockaddr_in );
	if (getsockname( sockMaster, ( struct sockaddr * )&sinMaster, &addrlen ) == -1)
	{
		printf( "getsockname() error: %s\n", strerror( errno ) );
		return -1;
	}
	printf( "Master socket binded to address:" );
	printIPPort( ( struct sockaddr * )&sinMaster );
	printf( "\n" );

	/*
	if (setSocketOption<int>( sockMaster, SOL_SOCKET, SO_REUSEADDR, 1 ) == -1)
	{
		fprintf( stderr, "Master socket setsockopt( SO_REUSEADDR ) error: %s\n", strerror( errno ) );
		return -1;
	}
	*/

	printf( "Set socket options SO_REUSEADDR/SO_REUSEPORT\n" );
	if (( allowReuseAddress( sockMaster ) ) == -1)
	{
		fprintf( stderr, "allowReuseAddr( sockMaster ) failed\n" );
		return -1;
	}
	
	printf( "Creating additional socket with options\n" );
	int sockAdd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if (sockAdd == -1)
	{
		fprintf( stderr, "socket() failed with error: %s\n", strerror( errno ) );
		return -1;
	}
	if (( allowReuseAddress( sockAdd ) ) == -1)
	{
		fprintf( stderr, "allowReuseAddr( sockAdd ) failed\n" );
		return -1;
	}

	printf( "Trying bind additional socket to master address\n" );
	if (( bind( sockAdd, ( struct sockaddr * )&sinMaster, sizeof( struct sockaddr_in ) ) ) == -1)
	{
		fprintf( stderr, "Additional socket bind() error: %s\n", strerror( errno ) );
		return -1;
	}

	struct sockaddr_in sinAdd;
	if (getsockname( sockAdd, ( struct sockaddr * )&sinAdd, &addrlen ) == -1)
	{
		printf( "getsockname() error: %s\n", strerror( errno ) );
		return -1;
	}
	printf( "Additional socket binded to address:" );
	printIPPort( ( struct sockaddr * )&sinAdd );
	printf( "\n" );

	int interval = 10;
	printf( "Keep sockets open for %d seconds...\n", interval );
	sleep( interval );
	close( sockMaster );
	close( sockAdd );
	printf( "Sockets closed\n", interval );

	return 0;
}
