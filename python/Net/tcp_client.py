import socket
import sys

if len(sys.argv) < 3:
    print('Usage: tcp_client.py host port [message]')
    exit()

server_address = (sys.argv[1], int(sys.argv[2]))
if len(sys.argv) > 3:
    data = sys.argv[3]
else:
    data = "Test message"

# Create TCP socket
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    # Connect to server and send data
    sock.connect(server_address)
    sock.sendall(bytes(data + "\n", "utf-8"))

    # Receive data from the server and shut down
    received = str(sock.recv(1024), "utf-8")

print("Sent:     {}".format(data))
print("Received: {}".format(received))