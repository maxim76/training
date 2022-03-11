#include <zmq.h>
#include <string.h>
#include <stdio.h>

int main( int argc, char* argv[] )
{
    if (argc != 3)
    {
        printf( "Usage: %s <server_ip> <server_port>\n", argv[0] );
        return -1;
    }

    printf( "Connecting to %s:%s\n", argv[1], argv[2] );
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);

    char server_addr[256];
#ifdef WIN32
    sprintf_s( server_addr, sizeof( server_addr ), "tcp://%s:%s", argv[1], argv[2] );
#else
    snprintf( server_addr, sizeof( server_addr ), "tcp://%s:%s", argv[1], argv[2] );
#endif
    zmq_connect( requester, server_addr );

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
        char buffer [10];
        printf ("Sending Hello %d…\n", request_nbr);
        zmq_send (requester, "Hello", 5, 0);
        zmq_recv (requester, buffer, 10, 0);
        printf ("Received World %d\n", request_nbr);
    }
    zmq_close (requester);
    zmq_ctx_destroy (context);
    return 0;
}
