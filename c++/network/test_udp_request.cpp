#include <stdio.h>

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#ifdef WIN32
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996) 
#endif

#include "udp_request.hpp"

int main( int argc, char *argv[] )
{
#ifdef WIN32
	// Initialize socket library
	WSADATA wsaData;
	if(WSAStartup( MAKEWORD( 1, 1 ), &wsaData ) != 0)
	{
		fprintf( stderr, "WSAStartup() failed\n" );
		return -1;
	}
	typedef int socklen_t;
#endif

	UDPRequest request("c7", 1234);
	char data[] = {"Hello"};
	request.send(1, data, 5);
}
