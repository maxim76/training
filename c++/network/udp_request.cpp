/*
	UDPRequest class 
*/

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#include <assert.h>
#include <ctime>
#include <stdio.h>

#include "udp_request.hpp"

UDPRequest::UDPRequest( const char *host, int port, int localPort ) : sendAttempts{ DEFAULT_SEND_ATTEMPTS }, sendTimeout{ DEFAULT_SEND_TIMEOUT }
{
	// Create connected UDP socket
	struct sockaddr_in serverAddr;
	memset( (void *)&serverAddr, 0, sizeof( serverAddr ) );
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons( port );
	serverAddr.sin_addr.s_addr = inet_addr( host );

	sockFD = socket( PF_INET, SOCK_DGRAM, 0 );

	// bind socket to local client's port
	struct sockaddr_in localAddr;
	memset( &localAddr, 0, sizeof( struct sockaddr_in ) );
	localAddr.sin_family = AF_INET;
	localAddr.sin_port = htons( localPort );

	if(localPort != 0)
	{
		if(bind( sockFD, (struct sockaddr *)&localAddr, sizeof( localAddr ) ) == -1)
		{
			fprintf( stderr, "bind() error: %s\n", strerror( errno ) );
			return;
		}
	}

	// Connect UDP socket
	if(connect( sockFD, (struct sockaddr *)&serverAddr, sizeof( struct sockaddr_in ) ) == -1)
	{
		fprintf( stderr, "connect() error: %s", strerror( errno ) );
		return;
	}

	// Create request array
	requests = new Request[MAX_REQUEST_COUNT];
	memset( requests, 0, sizeof( Request )*MAX_REQUEST_COUNT );
}

UDPRequest::~UDPRequest()
{
	delete [] requests;
}

bool UDPRequest::send( unsigned int req_id, char *data, size_t len )
{
	assert( len < 1500 );
	assert( req_id < MAX_REQUEST_COUNT );
	if(requests[req_id].isActive)
	{
		fprintf( stderr, "UDPRequest::send : req_id=%u. There is already active request\n", req_id );
	}
	requests[req_id].isActive = 1;
	memcpy( requests[req_id].data, (char *)&req_id, sizeof( req_id ) );
	memcpy( requests[req_id].data + sizeof( req_id ), data, len );
	requests[req_id].len = len + sizeof( req_id );
	requests[req_id].deadline = time( NULL ) + sendTimeout;
	requests[req_id].attemptNum = 0;
	int sentBytes = ::send( sockFD, requests[req_id].data, requests[req_id].len, 0 );
	if (sentBytes < 0)
	{
		fprintf( stderr, "UDPRequest::send : send() error: %s\n", strerror( errno ) );
		return false;
	}
	return true;
}