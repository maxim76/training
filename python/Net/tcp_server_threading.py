"""
TCP Server, based on ThreadedTCPServer
Example from https://docs.python.org/3/library/socketserver.html

Each request is served in a separate thread. 
TCPServer is used for basic socket communication, and ThreadingMixIn mix-in classes used to support asynchronous behaviour.

Development notes:
1. Create a request handler class by subclassing the BaseRequestHandler class and overriding its handle() method; this method will process incoming requests.
2. Instantiate one of the server classes, passing it the server’s address and the request handler class. It is recommended to use the server in a with statement. 
3. Call the handle_request() or serve_forever() method of the server object to process one or many requests. 
4. Call server_close() to close the socket (unless you used a with statement).

When inheriting from ThreadingMixIn for threaded connection behavior, you should explicitly declare how you want your threads to behave on an abrupt shutdown. 
The ThreadingMixIn class defines an attribute daemon_threads, which indicates whether or not the server should wait for thread termination. 
You should set the flag explicitly if you would like threads to behave autonomously; the default is False, meaning that Python will not exit until all 
threads created by ThreadingMixIn have exited.
"""
import socket
import threading
import socketserver

class ThreadedTCPRequestHandler(socketserver.BaseRequestHandler):

    def handle(self):
        data = str(self.request.recv(1024), 'ascii')
        cur_thread = threading.current_thread()
        response = bytes("{}: {}".format(cur_thread.name, data), 'ascii')
        self.request.sendall(response)

class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    pass

def client(ip, port, message):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((ip, port))
        sock.sendall(bytes(message, 'ascii'))
        response = str(sock.recv(1024), 'ascii')
        print("Received: {}".format(response))

if __name__ == "__main__":
    # Port 0 means to select an arbitrary unused port
    HOST, PORT = "localhost", 0

    server = ThreadedTCPServer((HOST, PORT), ThreadedTCPRequestHandler)
    with server:
        ip, port = server.server_address

        # Start a thread with the server -- that thread will then start one
        # more thread for each request
        server_thread = threading.Thread(target=server.serve_forever)
        # Exit the server thread when the main thread terminates
        server_thread.daemon = True
        server_thread.start()
        print("Server loop running in thread:", server_thread.name)

        client(ip, port, "Hello World 1")
        client(ip, port, "Hello World 2")
        client(ip, port, "Hello World 3")

        server.shutdown()