import socket
import sys

if len(sys.argv) != 3:
    print('Usage: udpserver.py serverIP serverPort')
    exit()

# Create UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

server_address = (sys.argv[1], int(sys.argv[2]))
message = 'This is the message.  It will be repeated.'

try:

    # Send data
    print( 'Sending "%s"' % message )
    sent = sock.sendto(message, server_address)

    # Receive response
    data, server = sock.recvfrom(4096)
    print( 'Received "%s"' % data )

finally:
    print( 'Closing socket' )
    sock.close()
