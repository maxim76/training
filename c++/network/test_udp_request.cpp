#include <assert.h>
#include <stdio.h>

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#endif

#ifdef WIN32
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996) 
#endif

#include "udp_request.hpp"

int main( int argc, char *argv[] )
{
	if(argc != 3)
	{
		fprintf( stderr, "Usage: %s <server_ip> <server_port>\n", argv[0] );
		return -1;
	}
	int serverPort = atoi( argv[2] );
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

	UDPRequest request( argv[1], serverPort );
	if(!request.ready())
	{
		fprintf( stderr, "UDPRequest initialization failed\n" );
		return -1;
	}
	char data[] = { "Hello" };

	// Test 1. Single request
	{
		request.send( 1, data, 5 );

		while(true)
		{
			request.update();
			bool isTimeout;
			unsigned int req_id;
			char data[MAX_DATAGRAM_SIZE];
			size_t len;

			if(request.recv( &req_id, &isTimeout, data, &len ))
			{
				if(isTimeout)
				{
					printf( "[%u] Timeout\n", req_id );
				}
				else
				{
					printf( "[%u] Received %d bytes\n", req_id, len );
				}
				break;
			}
#ifdef WIN32
			Sleep( 10 );
#else
			usleep( 10 * 1000 );
#endif
		}
	}
	
	// Test 2. Batch of requests
	int batchSize = 10;
	for(int i=0; i<batchSize; i++)
		request.send( i, data, 5 );
	int respCnt = 0;

	while(respCnt < batchSize)
	{
		request.update();
		bool isTimeout;
		unsigned int req_id;
		char data[MAX_DATAGRAM_SIZE];
		size_t len;

		if(request.recv( &req_id, &isTimeout, data, &len ))
		{
			if(isTimeout)
			{
				printf( "[%u] Timeout\n", req_id );
			}
			else
			{
				printf( "[%u] Received %d bytes\n", req_id, len );
			}
			++respCnt;
		}
#ifdef WIN32
		Sleep( 10 );
#else
		usleep( 10 * 1000 );
#endif
	}
	
	// Test 3. Attempt to send request when previous request is in progress
	bool result;
	printf("Sending first request, it expected to be sent successfully\n");
	result = request.send( 0, data, 5 );
	assert( result == true );

	printf("Sending second request, it expected to fail\n");
	result = request.send( 0, data, 5 );
	assert( result == false );
}
