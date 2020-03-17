import socket
import sys

if len(sys.argv) != 3:
    print('Usage: udpserver.py localIP localPort')
    exit()

# Create UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Bind the socket to the port
server_address = (sys.argv[1], int(sys.argv[2]))
print('Starting UDP server on %s port %s' % server_address)
sock.bind(server_address)

while True:
    data, address = sock.recvfrom(4096)
    
    print('Received %s bytes from %s' % (len(data), address))
    
    if data:
        sent = sock.sendto(data, address)
        print('Sent %s bytes back to %s' % (sent, address))
