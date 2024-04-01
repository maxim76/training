"""
Pipeline Demo. Receiving Part.

From https://rfc.zeromq.org/spec/30/:
The pattern is intended for task distribution, typically in a multi-stage pipeline where one or a few nodes push work to many workers,
and they in turn push results to one or a few collectors.
The pattern is mostly reliable insofar as it will not discard messages unless a node disconnects unexpectedly.
It is scalable in that nodes can join at any time.

The PUSH socket type talks to a set of anonymous PULL peers, sending messages using a round-robin algorithm.

The PULL socket type talks to a set of anonymous PUSH peers, receiving messages using a fair-queuing algorithm.

Usage example: 
push_pull_recv.py 5555
push_pull_send.py 5555
push_pull_send.py 5555
"""

import zmq
import sys

DEFAULT_PORT = "5555"
port = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_PORT

context = zmq.Context()

"""
A socket of type ZMQ_PULL is used by a pipeline node to receive messages from upstream pipeline nodes.
Messages are fair-queued from among all connected upstream nodes.
The zmq_send() function is not implemented for this socket type.
"""
socket = context.socket(zmq.PULL)

"""
In theory with ZeroMQ sockets, it does not matter which end connects and which end binds.
It's more convenient to use bind on stable part and use connect on dynamic part.
I.e. if one app push and many apps pull, then push part does bind. If multiple apps push and one does pull, then pull part does bind.
"""
#socket.bind("tcp://*:%s" % port)
socket.connect("tcp://localhost:%s" % port)

print("This App pulls messages from port %s" % port)

while True:
    print(socket.recv())
