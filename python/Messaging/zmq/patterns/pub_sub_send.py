"""
Publish-Subscribe Demo. Sending Part.

From https://rfc.zeromq.org/spec/29/:
The pattern is intended for event and data distribution, usually from a small number of publishers to a large number of subscribers, 
but also from many publishers to a few subscribers.

The PUB socket type provides basic one-way broadcasting to a set of subscribers.
Over TCP, it does filtering on outgoing messages but nonetheless a message will be sent multiple times over the network to reach multiple subscribers.

The SUB socket type provides a basic one-way listener for a set of publishers.

Usage example: 
pub_sub_recv.py 5555 5556
pub_sub_send.py 5555
pub_sub_send.py 5556
"""

import zmq
import time
import sys

DEFAULT_PORT = "5555"
port = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_PORT

context = zmq.Context()

"""
A socket of type ZMQ_PUB is used by a publisher to distribute data. Messages sent are distributed in a fan out fashion to all connected peers. 
The zmq_recv(3) function is not implemented for this socket type.
"""
socket = context.socket(zmq.PUB)

"""
In theory with ZeroMQ sockets, it does not matter which end connects and which end binds. However, in practice there are undocumented differences.
Bind the PUB and connect the SUB, unless your network design makes that impossible.
"""
socket.bind("tcp://*:%s" % port)

print("This App publish messages on port %s" % port)

while True:
    logString = "%(time)s App%(instance)s INFO: Log message" % {'time':time.strftime("%H.%M.%S"), 'instance':port}
    print("App sends log \"" + logString + "\"")

    """
    When a ZMQ_PUB socket enters the mute state due to having reached the high water mark for a subscriber, then any messages that would be sent
    to the subscriber in question shall instead be dropped until the mute state ends.
    The zmq_send() function shall never block for this socket type.
    """
    socket.send_string(logString)
    time.sleep(1)
