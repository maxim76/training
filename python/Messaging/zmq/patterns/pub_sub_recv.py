"""
Publish-Subscribe Demo. Receiving Part.

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
import sys

DEFAULT_PORTS = ["5555"]
ports = sys.argv[1:] if len(sys.argv) > 1 else DEFAULT_PORTS

context = zmq.Context()

"""
A socket of type ZMQ_SUB is used by a subscriber to subscribe to data distributed by a publisher. 
Initially a ZMQ_SUB socket is not subscribed to any messages, use the ZMQ_SUBSCRIBE option of zmq_setsockopt(3) to specify which messages to subscribe to.
The zmq_send() function is not implemented for this socket type.
"""
socket = context.socket(zmq.SUB)

"""
In theory with ZeroMQ sockets, it does not matter which end connects and which end binds. However, in practice there are undocumented differences.
Bind the PUB and connect the SUB, unless your network design makes that impossible.
"""
for port in ports:
    socket.connect("tcp://localhost:%s" % port)

socket.setsockopt_string(zmq.SUBSCRIBE, "")     # Empty subscription filter means subscribe to all

print("This Logger subscribed for messages from ports %s" % str(ports))

while True:
    print(socket.recv())
