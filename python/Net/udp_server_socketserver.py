"""
https://docs.python.org/3/library/socketserver.html
"""
import socketserver
import sys

class MyUDPHandler(socketserver.BaseRequestHandler):
    """
    This class works similar to the TCP handler class, except that
    self.request consists of a pair of data and client socket, and since
    there is no connection the client address must be given explicitly
    when sending data back via sendto().
    """

    def handle(self):
        data = self.request[0].strip()
        socket = self.request[1]
        print("{} wrote:".format(self.client_address[0]))
        print(data)
        socket.sendto(data.upper(), self.client_address)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print('Usage: udpserver.py localIP localPort')
        exit()

    server_address = (sys.argv[1], int(sys.argv[2]))
    print('Starting UDP server on %s port %s' % server_address)

    with socketserver.UDPServer(server_address, MyUDPHandler) as server:
        server.serve_forever()