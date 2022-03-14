import sys
import time
import zmq


context = zmq.Context()

# Socket to receive messages on
receiver = context.socket(zmq.PULL)
receiver.connect("tcp://localhost:5557")

# Socket to send messages to
sender = context.socket(zmq.PUSH)
sender.connect("tcp://localhost:5558")

poller = zmq.Poller()
#poller.register(sender, zmq.POLLOUT)
poller.register(receiver, zmq.POLLIN)

requests_cnt = 0

# Process tasks forever
while True:
    try:
        socks = dict(poller.poll())
    except KeyboardInterrupt:
        break

    if socks.get(receiver) == zmq.POLLIN:
        message = receiver.recv()
        # Simple progress indicator for the viewer
        sys.stdout.write('>')
        sys.stdout.flush()
        # Do the work
        if requests_cnt < 5:
            requests_cnt += 1
        else:
            requests_cnt = 0
            # Send responce
            # А что если в этот момент отправитель отвалится уже? может делать sender.send( ... , zmq.NOBLOCK) ?
            #sender.send(b'')
            try:
                sender.send(b"Notification", zmq.NOBLOCK)
                sys.stdout.write('<')
                sys.stdout.flush()
            except:
                sys.stdout.write('x')
                sys.stdout.flush()

