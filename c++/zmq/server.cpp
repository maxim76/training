//  Hello World server
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#ifdef WIN32
#include <stdlib.h>
#else
#include <unistd.h>
#endif
#include <assert.h>

int main( int argc, char* argv[] )
{
    if (argc != 3)
    {
        printf( "Usage: %s <server_ip> <server_port>\n", argv[0] );
        return -1;
    }

    //  Socket to talk to clients
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);

    char server_addr[256];
#ifdef WIN32
    sprintf_s( server_addr, sizeof( server_addr ), "tcp://%s:%s", argv[1], argv[2] );
#else
    snprintf( server_addr, sizeof( server_addr ), "tcp://%s:%s", argv[1], argv[2] );
#endif

    int rc = zmq_bind (responder, server_addr );
    assert (rc == 0);

    while (1) {
        char buffer [10];
        zmq_recv (responder, buffer, 10, 0);
        printf ("Received Hello\n");
#ifdef WIN32
        _sleep( 1000 );
#else
        sleep (1);          //  Do some 'work'
#endif
        zmq_send (responder, "World", 5, 0);
    }
    return 0;
}
