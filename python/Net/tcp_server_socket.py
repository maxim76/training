"""
TCP Server, based on the lowest level of API - sockets
Example from https://docs.python.org/3/library/socket.html

Single thread blocking server, can serve only 1 connection at a time.
Process requests synchronously; each request must be completed before the next request can be started.
Echoes back received messages
"""
import socket
import sys

if len(sys.argv) != 3:
    print('Usage: tcp_server_socket.py localIP localPort')
    exit()
server_address = (sys.argv[1], int(sys.argv[2]))
print('Starting socket based TCP server on %s port %s' % server_address)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind(server_address)
    s.listen(1)
    while(True):
        conn, addr = s.accept()
        with conn:
            print('Connected by', addr)
            while True:
                data = conn.recv(1024)
                if not data: break
                print('Received  : %s' % data)
                conn.sendall(data)
                print('Sent back : %s' % data)