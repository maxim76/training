"""
Pipeline Demo. Sending Part.

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

import sys
import time
import random
import zmq

DEFAULT_PORT = "5555"
port = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_PORT

context = zmq.Context()

"""
A socket of type ZMQ_PUSH is used by a pipeline node to send messages to downstream pipeline nodes. Messages are round-robined to all connected downstream nodes.
The zmq_recv() function is not implemented for this socket type.
"""
socket = context.socket(zmq.PUSH)

"""
In theory with ZeroMQ sockets, it does not matter which end connects and which end binds.
It's more convenient to use bind on stable part and use connect on dynamic part.
I.e. if one app push and many apps pull, then push part does bind. If multiple apps push and one does pull, then pull part does bind.
"""
socket.bind("tcp://*:%s" % port)
#socket.connect("tcp://localhost:%s" % port)

instance = random.randint(1,100)
counter = 1
print("This App %d pushes messages on port %s" % (instance, port))

while True:
    logString = "%(time)s App %(instance)d  Message #%(cnt)d" % {'time':time.strftime("%H.%M.%S"), 'instance':instance, 'cnt':counter}
    print("App sends message \"" + logString + "\"")

    """
    When a ZMQ_PUSH socket enters the mute state due to having reached the high water mark for all downstream nodes, or if there are no downstream nodes at all,
    then any zmq_send(3) operations on the socket shall block until the mute state ends or at least one downstream node becomes available for sending;
    messages are not discarded.
    """
    socket.send_string(logString)
    counter += 1
    time.sleep(1)
