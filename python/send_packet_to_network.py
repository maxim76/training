import binascii
import socket
import sys
from struct import *


HOST = 'toolbox'
PORT = 38913

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))

# Send registration message
# Format:
#	Packet Size		4 bytes
#	Message Type	4 bytes (0x108 - MESSAGE_LOGGER_REGISTER)
#   Logger Version	1 byte
#	UID				2 byte
#	PID				4 byte
#	ComponentName len	1 byte
#	Component name	len x byte

MESSAGE_LOGGER_REGISTER = 108
MESSAGE_LOGGER_VERSION  = 0x0a
LOGGER_ID = 0
UID = 10326
PID = 12345
componentName = 'TestComponent'

# Always use byte order command (for ex. '<') to avoid 4-byte padding
fmt = '<llbbhlb' + str(len(componentName)) + 's'
packetSize = calcsize(fmt)

buf = pack(fmt, packetSize, MESSAGE_LOGGER_REGISTER, MESSAGE_LOGGER_VERSION, LOGGER_ID, UID, PID, len(componentName), componentName)
print 'Sending "%s"' % binascii.hexlify(buf)
s.sendall( buf )
