import zmq
import random
import time


context = zmq.Context()

# Socket to send messages on
sender = context.socket(zmq.PUSH)
sender.bind("tcp://*:5557")

# Socket to receive messages on
receiver = context.socket(zmq.PULL)
receiver.bind("tcp://*:5558")

poller = zmq.Poller()
poller.register(sender, zmq.POLLOUT)
poller.register(receiver, zmq.POLLIN)

while True:
    print("New cycle")
    
    try:
        socks = dict(poller.poll())
    except KeyboardInterrupt:
        break

    if socks.get(receiver) == zmq.POLLIN:
        message = receiver.recv()
        print("Received something")

    if socks.get(sender) == zmq.POLLOUT:
        sender.send(b"Hello")
        print("Sent")


