import socket
import sys

if len(sys.argv) < 3:
    print('Usage: udp_client.py host port [message]')
    exit()

server_address = (sys.argv[1], int(sys.argv[2]))
if len(sys.argv) > 3:
    data = sys.argv[3]
else:
    data = "Test message"

# Create UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

try:

    # Send data
    print( 'Sending "%s"' % data )
    sent = sock.sendto(bytes(data + "\n", "utf-8"), server_address)

    # Receive response
    data, server = sock.recvfrom(4096)
    print( 'Received "%s"' % data )

finally:
    print( 'Closing socket' )
    sock.close()
