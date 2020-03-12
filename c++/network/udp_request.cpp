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
#include <string.h>
#include <errno.h>
#endif

#include <assert.h>
#include <ctime>
#include <fcntl.h>
#include <stdio.h>

#include "udp_request.hpp"

UDPRequest::UDPRequest( const char *host, int port, int localPort ) : 
	sendAttempts{ DEFAULT_SEND_ATTEMPTS }, 
	sendTimeout{ DEFAULT_SEND_TIMEOUT }
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

	// Set socket to be non-blocking
#ifdef WIN32
	unsigned long modeNonBlock = 1;
	if (ioctlsocket(sockFD, FIONBIO, &modeNonBlock) != NO_ERROR)
	{
		closesocket( sockFD );
#else
	int modeNonBlock = O_NONBLOCK;
	if (fcntl(sockFD,F_SETFL, modeNonBlock) < 0) {
	//if(ioctl( sockFD, FIONBIO, (char *)&on ) < 0) {
		close( sockFD );
#endif
		fprintf( stderr, "Could not set non-blocking mode\n" );
		return;
	}

	// Connect UDP socket
	if(connect( sockFD, (struct sockaddr *)&serverAddr, sizeof( struct sockaddr_in ) ) == -1)
	{
		fprintf( stderr, "connect() error: %s\n", strerror( errno ) );
		return;
	}

	// Create request array
	requests = new Request[MAX_REQUEST_COUNT];
	memset( requests, 0, sizeof( Request )*MAX_REQUEST_COUNT );

	ready_ = true;
}

UDPRequest::~UDPRequest()
{
	delete [] requests;
}

bool UDPRequest::send( TReqID req_id, char *data, size_t len )
{
	if(!ready_) return false;
	assert( len + sizeof( TReqID ) < MAX_DATAGRAM_SIZE );
	assert( req_id < MAX_REQUEST_COUNT );
	if(requests[req_id].isActive)
	{
		fprintf( stderr, "UDPRequest::send : req_id=%u. There is already active request\n", req_id );
	}
	requests[req_id].isActive = 1;
	memcpy( requests[req_id].data, (char *)&req_id, sizeof( req_id ) );
	memcpy( requests[req_id].data + sizeof( req_id ), data, len );
	requests[req_id].len = len + sizeof( req_id );
	requests[req_id].attemptNum = 0;
	return send( req_id );
}

bool UDPRequest::send( TReqID req_id )
{
	requests[req_id].deadline = time( NULL ) + sendTimeout;
	requests[req_id].attemptNum++;
	int sentBytes = ::send( sockFD, requests[req_id].data, requests[req_id].len, 0 );
	if(sentBytes < 0)
	{
		fprintf( stderr, "UDPRequest::send : send() error: %s\n", strerror( errno ) );
		return false;
	}
	return true;
}

bool UDPRequest::tryReceiveFromSocket( TReqID *req_id, char *data, size_t *len )
{
	char buffer[MAX_DATAGRAM_SIZE];
	int recvBytes = ::recv( sockFD, buffer, MAX_DATAGRAM_SIZE, 0 );
	//printf("UDPRequest::tryReceiveFromSocket : recvBytes = %d\n", recvBytes );
	if(recvBytes < 0) return false;
	if(recvBytes < sizeof( TReqID ))
	{
		fprintf( stderr, "UDPRequest::recv : received buffer is too short, %d bytes\n", recvBytes );
		return false;
	}
	TReqID * reqPtr = (TReqID *)buffer;
	*req_id = *reqPtr;

	if(*req_id < MAX_REQUEST_COUNT)
	{
		memcpy( data, &(buffer[sizeof( TReqID )]), recvBytes );
		*len = recvBytes - sizeof( TReqID );
		return true;
	}
	else
	{
		fprintf( stderr, "UDPRequest::recv : unexpected req_id %lu\n", *req_id );
		return false;
	}
}

/*
Check any changes on request status. I.e. responce is ready, or request is timed out
*/
void UDPRequest::update()
{
	socketProcessed = false;
	currentReqIndex = 0;
}

bool UDPRequest::recv( TReqID *req_id, bool *isTimeout, char *data, size_t *len )
{
	if(!ready_) return false;
	if(!socketProcessed)
	{
		if(tryReceiveFromSocket( req_id, data, len ))
		{
			requests[*req_id].isActive = false;
			*isTimeout = false;
			return true;
		}
		else
		{
			socketProcessed = true;
		}
	}

	// No more data available in socket, check if amy timeout occured
	while(currentReqIndex < MAX_REQUEST_COUNT)
	{
		if(requests[currentReqIndex].isActive)
		{
			if(requests[currentReqIndex].deadline < time( NULL ))
			{
				if(requests[currentReqIndex].attemptNum < sendAttempts)
				{
					// Timeout. Resend packet
					printf( "Timeout. Resend\n" );
					send( currentReqIndex );
				}
				else
				{
					// Timout and all resend attempt already used
					printf( "Timeout. Final attempt failed\n" );
					requests[currentReqIndex].isActive = false;
					*req_id = currentReqIndex;
					*isTimeout = true;
					return true;
				}
			}
		}
		++currentReqIndex;
	}

	// No events, neither receive from socket nor timeouts
	return false;
}
