"""
TCP Server, based on socketserver.TCPServer class
Example from https://docs.python.org/3/library/socketserver.html

Single thread blocking server, can serve only 1 connection at a time.
Echoes back received messages
"""
import socketserver
import sys

class MyTCPHandler(socketserver.BaseRequestHandler):
    """
    The request handler class for server.
    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """

    def handle(self):
        # self.request is the TCP socket connected to the client
        self.data = self.request.recv(1024).strip()
        print("{} wrote:".format(self.client_address[0]))
        print(self.data)
        # just send back the same data, but upper-cased
        self.request.sendall(self.data.upper())

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print('Usage: tcp_socket_server.py localIP localPort')
        exit()

    server_address = (sys.argv[1], int(sys.argv[2]))
    print('Starting socketserver based TCP server on %s port %s' % server_address)

    with socketserver.TCPServer(server_address, MyTCPHandler) as server:
        # Activate the server; this will keep running until you
        # interrupt the program with Ctrl-C
        server.serve_forever()