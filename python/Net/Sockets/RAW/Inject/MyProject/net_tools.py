"""This helper module provides capabilities to inject 
TCP/UDP/ICMP packets into network
"""

import socket
import struct
import random
import sys
import binascii

# The checksum function:
def checkSum( data ):
    s = 0
    n = len(data) % 2
    for i in range(0, len(data)-n, 2):
        s+= ord(data[i]) + (ord(data[i+1]) << 8)
    if n:
        s+= ord(data[i+1])
    while (s >> 16):
        s = (s & 0xFFFF) + (s >> 16)
    s = ~s & 0xffff
    return s

class IP( object ):
	""" Implements IP layer encapsulation """
	def __init__( self, srcIP, dstIP, proto=socket.IPPROTO_TCP, totalLen=0 ):
		"""
		Initialises IP packet

		@param srcIP: Source host IP address
		@param dstIP: Destination host IP address
		@param proto: upper layer protocol (default is TCP)
		@param totalLen: length of payload data to be included into IP packet
		"""
		self.version = 4
		self.ihl = 5			# Internet Header Length
		self.tos = 0			# Type of Service
		self.tl = 20 + totalLen	# total length also can be filled by kernel
		self.id = random.randint( 0, 65535 )
		self.flags = 0
		self.offset = 0
		self.ttl = 255
		self.protocol = proto
		self.checkSum = 0 # will be filled by kernel
		self.srcIP = socket.inet_aton( srcIP )
		self.dstIP = socket.inet_aton( dstIP )
	def pack( self ):
		"""
		Formats IP packet in a byte-order suitable for sending to socket

		@return formatted packet as a Python string
		"""
		ver_ihl = ( self.version << 4 ) + self.ihl
		flags_offset = ( self.flags << 13 ) + self.offset
		ip_header = struct.pack( "!BBHHHBBH4s4s",
			ver_ihl,
			self.tos,
			self.tl,
			self.id,
			flags_offset,
			self.ttl,
			self.protocol,
			self.checkSum,
			self.srcIP,
			self.dstIP )
		return ip_header

class TCP( object ):
	""" Implenets TCP protocol encapsulation """
	def __init__( self, srcPort, dstPort ):
		"""
		Initialises TCP segment

		@param srcPort: Source port
		@param dstPort: Destination port
		"""
		self.srcPort = srcPort
		self.dstPort = dstPort
		self.seqn = 0
		self.ackn = 0
		self.offset = 5 # Data offset: 5x4 = 20 bytes
		self.reserved = 0
		self.urg = 0
		self.ack = 0
		self.psh = 1
		self.rst = 0
		self.syn = 0
		self.fin = 0
		self.window = socket.htons(5840)
		self.checkSum = 0
		self.urgp = 0
	def pack( self, srcIP, dstIP, payload ):	# input param are for checksum
		"""
		Formats TCP segment in a byte-order suitable for sending to socket

		@param srcIP: Source host IP address (used for checksum calculation)
		@param dstIP: Destination host IP address (used for checksum calculation)
		@param payload: upper layer data (used for checksum calculation)

		@return formatted packet as a Python string
		"""
		data_offset = (self.offset << 4) + 0
		flags = self.fin + (self.syn << 1) + (self.rst << 2) + (self.psh << 3) \
			+ (self.ack << 4) + (self.urg << 5)
		# Pack TCP header with zeroed checksum
		tcpHdr = struct.pack('!HHLLBBHHH',
			self.srcPort,
			self.dstPort,
			self.seqn,
			self.ackn,
			data_offset,
			flags, 
			self.window,
			self.checkSum,
			self.urgp)
		# Create pseudo header for checksum calculation
		pseudoHdr = struct.pack( "!4s4sBBH",
			socket.inet_aton(srcIP),
			socket.inet_aton(dstIP),
			0,					# reserved
			socket.IPPROTO_TCP, # protocol
			len( tcpHdr ) + len( payload ) )
		pseudoHdr = pseudoHdr + tcpHdr
		tcpChecksum = checkSum( pseudoHdr + str( payload ) )
		# Repack TCP header with correct checksum
		tcpHdr = struct.pack("!HHLLBBH",
			self.srcPort,
			self.dstPort,
			self.seqn,
			self.ackn,
			data_offset,
			flags,
			self.window) + \
			struct.pack('<H', tcpChecksum) + \
			struct.pack('!H', self.urgp)
		return tcpHdr

class UDP( object ):
	""" Implenets UDP protocol encapsulation """
	def __init__( self, srcPort, dstPort ):
		"""
		Initialises UDP segment

		@param srcPort: Source port
		@param dstPort: Destination port
		"""
		self.srcPort  = srcPort
		self.dstPort  = dstPort
	def pack( self, srcIP, dstIP, payload ):	# input param are for checksum
		"""
		Formats UDP segment in a byte-order suitable for sending to socket

		@param srcIP: Source host IP address (used for checksum calculation)
		@param dstIP: Destination host IP address (used for checksum calculation)
		@param payload: upper layer data (used for checksum calculation)

		@return formatted packet as a Python string
		"""
		totalLen = 8 + len( payload )
		# Pack UDP header with zeroed checksum
		udpChecksum = 0
		udpHdr = struct.pack( '!HHHH',
			self.srcPort, self.dstPort, totalLen, udpChecksum )
		# Create pseudo header for checksum calculation
		pseudoHdr = struct.pack( "!4s4sBBH",
			socket.inet_aton(srcIP),
			socket.inet_aton(dstIP),
			0,					# reserved
			socket.IPPROTO_UDP, # protocol
			len( udpHdr ) + len( payload ) )
		pseudoHdr = pseudoHdr + udpHdr
		udpChecksum = checkSum( pseudoHdr + str( payload ) )
		# Pack UDP header with correct checksum
		udpHdr = struct.pack( '!HHH', self.srcPort, self.dstPort, totalLen ) + \
			struct.pack('<H', udpChecksum)
		return udpHdr

class ICMP( object ):
	""" Implenets ICMP protocol encapsulation """
	def __init__( self, icmp_type, icmp_code ):
		"""
		Initialises ICMP packet
		"""
		self.icmp_type = icmp_type
		self.icmp_code = icmp_code
	def pack( self, payload = '' ):
		"""
		Formats ICMP packet in a byte-order suitable for sending to socket

		@param payload: upper layer data (used for checksum calculation)

		@return formatted packet as a Python string
		"""
		# Create pseudo header for checksum calculation
		pseudoHdr = struct.pack( '!BBHL', self.icmp_type, self.icmp_code, 0, 0 )
		icmpChecksum = checkSum( pseudoHdr + str( payload ) )
		# Pack ICMP header with correct checksum
		icmpHdr = struct.pack( '!BB', self.icmp_type, self.icmp_code ) + \
			struct.pack( '<HL', icmpChecksum, 0 )	# checksum is little endian
		return icmpHdr

class NetInject( object ):
	"""
	Implements facility of artificial packets injection into network
	"""
	def injectICMP( self, srcIP, srcPort, dstIP, dstPort, icmpType, icmpCode, data ):
		"""
		Build and inject ICMP packet into network.
		Note: host addresses and ports should be filled from point of view of 
		imagined original packet, that would be answered with ICMP notification
		I.e. source and destination should be swapped.
		Example: if ICMP is required to notify 10.1.1.1 that 10.2.2.2 is 
		unavailable, then srcIP set to 10.1.1.1 and dstIP set to 10.2.2.2

		@param srcIP: Source host IP address (see comment above)
		@param dstIP: Destination host IP address (see comment above)
		@param srcPort: Source port (see comment above)
		@param dstPort: Destination port (see comment above)
		@param icmpType: Type of message. Example: 3 is for 'Destination unreachable'
		@param icmpCode: Code of message. Example: 1 is for 'Destination host unreachable'
		@param data: L5-L7 layer data, that would be sent in original packet
		"""
		s = socket.socket( socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_RAW )

		payload = bytearray.fromhex( data )				# [data]

		hdrIncludedUDP = UDP( srcPort, dstPort )
		payload = hdrIncludedUDP.pack( srcIP, dstIP, payload) + payload#[UDP/data]

		hdrIncludedIP = IP( srcIP, dstIP, socket.IPPROTO_UDP, len( payload ) )
		payload = hdrIncludedIP.pack() + payload		# [IP/UDP/data]

		hdrICMP = ICMP( icmpType, icmpCode )
		payload = hdrICMP.pack( payload ) + payload		# ICMP/[IP/UDP/data]

		# swap src and dst for ICMP packet
		hdrIP = IP( dstIP, srcIP, socket.IPPROTO_ICMP, len( payload ) )
		payload = hdrIP.pack() + payload				# IP/ICMP/[IP/UDP/data]

		s.sendto( payload, (srcIP, 0) )

	def injectUDP( self, srcIP, srcPort, dstIP, dstPort, data ):
		"""
		Build and inject UDP packet into network.

		@param srcIP: Source host IP address
		@param dstIP: Destination host IP address
		@param srcPort: Source port
		@param dstPort: Destination port
		@param data: L5-L5 payload
		"""
		s = socket.socket( socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_RAW )

		payload = bytearray.fromhex( data )						# [data]

		hdrUDP = UDP( srcPort, dstPort )
		payload = hdrUDP.pack( srcIP, dstIP, payload ) + payload# UDP/[data]

		hdrIP = IP( srcIP, dstIP, socket.IPPROTO_UDP, len( payload ) )
		payload = hdrIP.pack() + payload						# IP/UDP/[data]

		s.sendto( payload, ( dstIP, dstPort ) )

	def injectTCP( self, srcIP, srcPort, dstIP, dstPort, data ):
		"""
		Build and inject TCP packet into network.

		@param srcIP: Source host IP address
		@param dstIP: Destination host IP address
		@param srcPort: Source port
		@param dstPort: Destination port
		@param data: L5-L5 payload
		"""
		s = socket.socket( socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_RAW )

		payload = bytearray.fromhex( data )						# [data]

		hdrTCP = TCP( srcPort, dstPort )
		payload = hdrTCP.pack( srcIP, dstIP, payload ) + payload# TCP/[data]

		hdrIP = IP( srcIP, dstIP, socket.IPPROTO_TCP, len( payload ) )
		payload = hdrIP.pack() + payload						# IP/TCP/[data]

		s.sendto( payload, ( dstIP, dstPort ) )
