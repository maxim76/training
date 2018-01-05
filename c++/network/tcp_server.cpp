/*
	Simple TCP Server. Waits for incoming connection. 
	After getting new connection, sends message to client and closes connection.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define BACKLOG 10 // how many pending connections queue will hold

void printIPPort( struct sockaddr *sa );		// extern from common.cpp


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


/*
	This func removes all child zombies
*/
void sigchld_handler(int s)
{
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;
	// Wait for any child and do it as long as still have one, no hang
	while (waitpid( -1, NULL, WNOHANG ) > 0);
	errno = saved_errno;
}


int main( int argc, char *argv[] )
{
	if (argc != 3)
	{
		fprintf( stderr, "Usage: %s <server_ip> <server_port>\n", argv[0] );
		return -1;
	}

	int listeningSocket, clientSocket;
	struct addrinfo hints, *servinfo, *ptr;
	struct sockaddr_storage clientAddr;
	socklen_t sin_size;
	struct sigaction sa;
	int status;

	memset( &hints, 0, sizeof hints );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (( status = getaddrinfo( NULL, argv[2], &hints, &servinfo ) ) != 0) {
		fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( status ) );
		return -1;
	}

	// loop through all the results and bind to the first we can
	for (ptr = servinfo; ptr != NULL; ptr = ptr->ai_next) {
		if (( listeningSocket = socket( ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol ) ) == -1) {
			fprintf( stderr, "socket() error: %s\n", strerror( errno ) );
			continue;
		}
		// Avoid "socket in use" issue after program restart
		if (setSocketOption<int>( listeningSocket, SOL_SOCKET, SO_REUSEADDR, 1 ) == -1)
		{
			fprintf( stderr, "setsockopt( SO_REUSEADDR ) error: %s\n", strerror( errno ) );
			return -1;
		}

		if (bind( listeningSocket, ptr->ai_addr, ptr->ai_addrlen ) == -1) {
			close( listeningSocket );
			fprintf( stderr, "bind() error: %s\n", strerror( errno ) );
			continue;
		}
		break;
	}
	freeaddrinfo( servinfo ); // all done with this structure

	if (ptr == NULL) {
		fprintf( stderr, "Could not open/bind socket\n" );
		return -1;
	}

	if (listen( listeningSocket, BACKLOG ) == -1) {
		fprintf( stderr, "listen() error: %s\n", strerror( errno ) );
		return -1;
	}

	// Enable reap all zombies after childs are terminated
	sa.sa_handler = sigchld_handler;
	sigemptyset( &sa.sa_mask );
	sa.sa_flags = SA_RESTART;
	if (sigaction( SIGCHLD, &sa, NULL ) == -1) {
		fprintf( stderr, "sigaction() error: %s\n", strerror( errno ) );
		return -1;
	}

	printf( "Waiting for connections...\n" );
	pid_t cpid;
	while (1) { // main accept() loop
		sin_size = sizeof clientAddr;
		clientSocket = accept( listeningSocket, ( struct sockaddr * )&clientAddr, &sin_size );
		if (clientSocket == -1) {
			fprintf( stderr, "accept() error: %s\n", strerror( errno ) );
			continue;
		}

		printf( "\nGot connection from " ); printIPPort( ( struct sockaddr * )&clientAddr );	printf( "\n" );

		if ((cpid = fork()) == -1)
		{
			fprintf( stderr, "fork() error: %s\n", strerror( errno ) );
			return -1;
		}

		if (cpid == 0)	// this is the child process
		{ 
			close( listeningSocket );
			const char *message = "Hello from tcp server";
			int sentBytes;
			if ((sentBytes = send( clientSocket, message, strlen( message ), 0 )) == -1)
			{
				fprintf( stderr, "send() error: %s\n", strerror( errno ) );
			}
			else
			{
				printf( "OUT[%d]: %s\n", sentBytes, message );
			}
			close( clientSocket );
			printf( "Connection closed\n" );
			exit( 0 );
		}
		else // this is parent process
		{
			close( clientSocket );
		}
	}
	return 0;
}
