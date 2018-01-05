/*
	UDP sender. Sends UDP packet(s) to destination
	Data to send can be provided as:
		- text message
		- hexadecimal dump
		- binary file. If several files specified, each will be send in a separate packet
		- generated automatically using passed IP packet size

	Allows to set specific socket options and sending repetitions
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>
#include <list>
#include <unistd.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
#else
#include <errno.h> 
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#ifdef _WIN32
#pragma comment(lib,"Ws2_32.lib")
#endif

#define MAX_STR_SIZE	128
#define MAX_PACKET_SIZE	9000

void showSyntax() {
	printf( "\nudp_send. (Build 1)\n" );
	printf( "Sends UDP packet\n\n" );
	printf( "Syntax: \n"
		"udp_send.exe -d <destination_host:destination_port>\n"
		"             [-l <local port>]\n"
		"             [-m <text message to send>] | [-b <hex buffer to be send>] | [-s <IP packet size>] | [-f \"<list of binary files>\"]\n"
		"             [-r <repeat time>]\n"
		"             [-w <wait between repetitions, sec>]\n"
		"             [-o <socket_option=value>]\n" );
	printf( "\nudp_send.exe -? to get help: \n" );
}

// Vars for keeping parameters
char host[128];
int port;
int option = -1;
int optionVal = 0;
bool destProvided = false;
int repeatCnt = 1;
int waitTime = 0;
int localPort = 0;

struct Packet
{
	char *data;
	int len;
};
std::list<Packet *>	packets;

bool readCLIParameters( int argc, char* argv[] )
{
	for (int i = 1; i < argc; i++)
	{
		if (( strcmp( argv[i], "-?" ) == 0 ) || ( strcmp( argv[i], "-h" ) == 0 ))
		{
			showSyntax();
			printf( "Linux IP options list:\n"
				"\tIP_MTU_DISCOVER	10\n" );

			printf( "IP_MTU_DISCOVER values\n"
				"\tIP_PMTUDISC_DONT    0\n"
				"\tIP_PMTUDISC_WANT    1\n"
				"\tIP_PMTUDISC_DO      2\n"
				"\tIP_PMTUDISC_PROBE   3\n" );

			exit(0);
		}

		// All params other than "-?" and "-h" have param name and param value. Get it.
		const char *arg = argv[i + 1];
		int argLen = strlen( arg );

		if (strcmp( argv[i], "-d" ) == 0)
		{
			int pos = 0;
			while (( pos < argLen ) && ( arg[pos] != ':' ))
			{
				host[pos] = argv[i + 1][pos];
				pos++;
			}
			host[pos] = 0;
			if (( pos == argLen ) || ( arg[pos] != ':' ))
			{
				printf( "Port is missing\n" );
				return false;
			}
			port = atoi( &arg[++pos] );
			if (port == 0)
			{
				printf( "Port is missing\n" );
				return false;
			}
			printf( "Host = %s Port = %i\n", host, port );
			destProvided = true;
			i++;
		}
		else if (strcmp( argv[i], "-l" ) == 0)
		{
			localPort = atoi( argv[i + 1] );
			printf( "Local port = %d\n", localPort );
			i++;
		}
		else if (strcmp( argv[i], "-m" ) == 0)
		{
			printf( "Message to send = %s\n", argv[i + 1] );
			int packetSize = strlen( argv[i + 1] );

			Packet *packet = new Packet;
			packet->data = new char[packetSize];
			packet->len = packetSize;
			memcpy( packet->data, argv[i + 1], packetSize );
			packets.push_back( packet );

			i++;
		}
		else if (strcmp( argv[i], "-b" ) == 0)
		{
			char *pBuf = new char[argLen+1];
			int argPos = 0;
			int bufPos = 0;
			while (argPos < argLen)
			{
				if (isxdigit( arg[argPos] )) pBuf[bufPos++] = arg[argPos];
				argPos++;
			}
			pBuf[bufPos] = 0;
			if (bufPos % 2 == 1)
			{
				printf( "Buffer should have even number of hex digits\n" );
				return false;
			}
			int bufLen = bufPos;
			printf( "Buffer = %s\n", pBuf );
			i++;

			int packetSize = bufLen / 2;
			Packet *packet = new Packet;
			packet->data = new char[packetSize];
			packet->len = packetSize;
			for (int i = 0; i < bufLen / 2; i++)
			{
				packet->data[i] = ( ( pBuf[i * 2] - 48 ) << 4 ) | ( pBuf[i * 2 + 1] - 48 );
			}
			packets.push_back( packet );
		}
		else if (strcmp( argv[i], "-s" ) == 0)
		{
			int packetSize = atoi( argv[i + 1] );
			printf( "IP packet size = %d\n", packetSize );
			i++;

			Packet *packet = new Packet;
			packet->data = new char[packetSize];
			packet->len = packetSize;
			memset( packet->data, 0xff, packetSize );
			packets.push_back( packet );
		}
		else if (strcmp( argv[i], "-f" ) == 0)
		{
			const char *pos = arg;
			int fileNameLen = 0;
			char fileName[MAX_STR_SIZE];
			while (*pos != 0)
			{
				if ( !isspace( pos[0] ))
				{
					fileName[fileNameLen++] = *pos;
				}

				if ((isspace( pos[1] )) || (pos[1]==0))
				{
					if (fileNameLen > 0)
					{
						fileName[fileNameLen] = 0;
						printf( "Read binary file: %s\n", fileName );
						FILE *f = fopen( fileName, "rb" );
						if (f == NULL)
						{
							printf( "fopen() error. Cannot open file\n" );
							return false;
						}
						char *buf = new char[MAX_PACKET_SIZE];
						int readSize = fread( buf, 1, MAX_PACKET_SIZE, f );
						if (readSize <= 0)
						{
							printf( "Nothing is read from file\n" );
							return false;
						} 
						else
						{
							Packet *packet = new Packet;
							packet->data = new char[readSize];
							packet->len = readSize;
							memcpy( packet->data, buf, readSize );
							packets.push_back( packet );
						}
						fileNameLen = 0;
					}
				} 
				++pos;
			}
			i++;
		}
		else if (strcmp( argv[i], "-r" ) == 0)
		{
			repeatCnt = atoi( argv[i + 1] );
			printf( "Number to repeat = %d\n", repeatCnt );
			i++;
		}
		else if (strcmp( argv[i], "-w" ) == 0)
		{
			waitTime = atoi( argv[i + 1] );
			printf( "Time to wait = %d\n", waitTime );
			i++;
		}
		else if (strcmp( argv[i], "-o" ) == 0)
		{
			char *ptr;
			option = strtol( arg, &ptr, 10 );
			if (*ptr != '=')
			{
				printf( "Option value is not provided\n" );
				return false;
			}
			optionVal = strtol( ++ptr, &ptr, 10 );
			printf( "IP socket option to be set : %d = %d\n", option, optionVal );
			i++;
		}
		else
		{
			printf( "Wrong argument: %s\n", argv[i] );
			return false;
		}
	}
	return true;
}

int main( int argc, char* argv[] )
{
	if (!readCLIParameters( argc, argv ) || !destProvided)
	{
		showSyntax();
		return -1;
	}
	int result;

#ifdef _WIN32
	SOCKET	sock = INVALID_SOCKET;
#else
	int		sock;
#endif
	struct addrinfo hints, *servinfo = NULL;
#ifdef _WIN32
	// Socket inialization and opening
	WSADATA wsaData;

	// Initialize Winsock
	result = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
	if (result != 0)
	{
		fprintf( stderr, "WSAStartup failed with error: %d\n", result );
		return -1;
	}

	sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if (sock == INVALID_SOCKET)
	{
		fprintf( stderr, "socket() failed with error: %ld\n", WSAGetLastError() );
		WSACleanup();
		return -1;
	}
#else
	sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if (sock == -1)
	{
		fprintf( stderr, "socket() failed with error: %s\n", strerror( errno ) );
		return -1;
	}
#endif

	memset( &hints, 0, sizeof hints );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	const int strBufSize = 16;
	char sPort[strBufSize];
	sprintf( sPort, "%d", port );
	getaddrinfo( host, sPort, &hints, &servinfo );
	if (( result = getaddrinfo( host, sPort, &hints, &servinfo ) ) != 0)
	{
		fprintf( stderr, "getaddrinfo() error: %s\n", gai_strerror( result ) );
		return -1;
	}

	// bind socket to local client's port
	if (localPort > 0)
	{
		struct sockaddr_in localAddr;
		memset( &localAddr, 0, sizeof( sockaddr_in ) );
		localAddr.sin_family = servinfo->ai_addr->sa_family;
		localAddr.sin_port = htons( localPort );

		if (bind( sock, ( struct sockaddr * )&localAddr, sizeof( localAddr ) ) == -1)
		{
			fprintf( stderr, "bind() error: %s\n", strerror( errno ) );
			return -1;
		}
	}

	// Set socket options
	int optLen = sizeof( int );
	if (option != -1)
	{
		if (setsockopt( sock, IPPROTO_IP, option, (char *)&optionVal, optLen ) == -1)
		{
			fprintf( stderr, "setsockopt() failed with error: %s\n", strerror( errno ) );
			return -1;
		}
	}
		

	printf( "\n" );

	int packetCnt = packets.size();
	if (packetCnt == 0)
	{
		printf( "Nothing to send\n" );
		return -1;
	}

	for (int i = 0; i < repeatCnt; i++)
	{
		if (repeatCnt > 1)
		{
			printf( "Repetition %d is started\n", i + 1 );
		}

		int cnt = 1;
		for (Packet *p : packets)
		{
			printf( "Sending packet %d of %d\n", cnt++, packetCnt );
			if (sendto( sock, p->data, p->len, 0, servinfo->ai_addr, servinfo->ai_addrlen ) == -1)
			{
				fprintf( stderr, "sendto() error: %s\n", strerror( errno ) );
			}
		}

		if (repeatCnt > 1)
		{
			sleep( waitTime );
		}
	}
	printf( "Done\n" );
	return 0;
}
