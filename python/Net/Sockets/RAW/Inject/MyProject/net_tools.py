"""This helper module provides capabilities to inject 
TCP/UDP/ICMP packets into network

Functions defined here require *ROOT* privileges
"""

import socket
import struct
import random
import sys
import binascii
import re


# The checksum function:
def checkSum( data ):
	"""
	Calculates internet checksum

	@param data: data to be used for checksum calculation, passed as a bytearray
	"""
	s = 0
	n = len( data ) % 2
	for i in range( 0, len( data )-n, 2 ):
		s+= data[i] + (data[i+1] << 8)
	if n:
		s+= data[i+1]
	while ( s >> 16 ):
		s = ( s & 0xFFFF ) + ( s >> 16 )
	s = ~s & 0xffff
	return s

def splitAddress( address ):
	"""
	Splits address from 'IP:port' string to separate IP and port
	@param address: Source address in form 'IP:port'
	@return host, port
	"""
	host = re.split( ':', address )[0]
	port = int( re.split( ':', address )[1] )
	return host, port

class AbstractL4Packet( object) :
	"""Creates L4 packet"""
	def __init__( self, srcAddress, dstAddress ):
		"""
		Default root constructor for all L4 protocols

		@param srcAddress: Source address in form 'IP:port'
		@param dstAddress: Destination address in form 'IP:port'
		"""
		self.srcHost, self.srcPort = splitAddress( srcAddress )
		self.dstHost, self.dstPort = splitAddress( dstAddress )

	def createIPPacket( self ):
		"""
		Virtual, must be overriden
		"""
		pass

	def inject( self ):
		"""Injects IP packet into network"""
		self.createIPPacket().inject( self.dstPort )

class IPPacket( object ):
	""" Implements IP layer encapsulation """

	def __init__( self, srcHost, dstHost, proto, payload ):
		"""
		Initialises IP packet

		@param srcHost: Source host IP address
		@param dstHost: Destination host IP address
		@param proto: upper layer protocol
		@param payload: payload data to be included into IP packet
		"""
		self.version = 4
		self.ihl = 5					# Internet Header Length
		self.tos = 0					# Type of Service
		self.tl = 20 + len( payload )	# total len also can be filled by kernel
		self.id = random.randint( 0, 65535 )
		self.flags = 0
		self.offset = 0
		self.ttl = 255
		self.protocol = proto
		self.checkSum = 0 # will be filled by kernel
		self.srcHost = srcHost
		self.dstHost = dstHost
		self.payload = payload
		print("IPPacket::IPPacket : passed IP payload %s" % ( ' '.join('%02X' % ch for ch in payload)))

	def packHeader( self ):
		"""
		Formats IP header in a byte-order suitable for sending to socket

		@return formatted header as a Python string
		"""
		ver_ihl = ( self.version << 4 ) + self.ihl
		flags_offset = ( self.flags << 13 ) + self.offset
		hdrIP = struct.pack( "!BBHHHBBH4s4s",
			ver_ihl,
			self.tos,
			self.tl,
			self.id,
			flags_offset,
			self.ttl,
			self.protocol,
			self.checkSum,
			socket.inet_aton( self.srcHost ),
			socket.inet_aton( self.dstHost ) )
		return hdrIP

	def pack( self ):
		""" 
		Formats IP packet in a byte-order suitable for sending to socket

		@return formatted IP packet as a Python string
		"""
		return self.packHeader() + self.payload
	
	def inject( self, dstPort ):
		"""
		Injects IP packet into network

		@param dstPort: Destination Port
		"""
		data = self.pack()
		print("IPPacket::inject: %s" % ( ' '.join('%02X' % ch for ch in data)))
		s = socket.socket( socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_RAW )
		s.sendto( data, ( self.dstHost, dstPort ) )


class TCPPacket( AbstractL4Packet ):
	""" Implemets TCP protocol encapsulation """

	def __init__( self, srcAddress, dstAddress, payload ):
		"""
		Initialises TCP segment

		@param srcAddress: Source address in form 'IP:port'
		@param dstAddress: Destination address in form 'IP:port'
		@param payload:    payload data to be included into TCP segment
		"""
		super( TCPPacket, self ).__init__( srcAddress, dstAddress )
		self.payload  = payload
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
		self.window = socket.htons( 5840 )
		self.checkSum = 0
		self.urgp = 0

	def packHeader( self ):	# input param are for checksum
		"""
		Formats TCP header in a byte-order suitable for sending to socket

		@return formatted header as a Python string
		"""
		data_offset = ( self.offset << 4 ) + 0
		flags = self.fin + ( self.syn << 1 ) + ( self.rst << 2 ) \
			+ ( self.psh << 3 ) + (self.ack << 4) + (self.urg << 5)
		# Pack TCP header with zeroed checksum
		tcpHdr = struct.pack( '!HHLLBBHHH',
			self.srcPort,
			self.dstPort,
			self.seqn,
			self.ackn,
			data_offset,
			flags, 
			self.window,
			self.checkSum,
			self.urgp )
		# Create pseudo header for checksum calculation
		pseudoHdr = struct.pack( "!4s4sBBH",
			socket.inet_aton( self.srcHost ),
			socket.inet_aton( self.dstHost ),
			0,					# reserved
			socket.IPPROTO_TCP, # protocol
			len( tcpHdr ) + len( self.payload ) )
		pseudoHdr = pseudoHdr + tcpHdr
		tcpChecksum = checkSum( pseudoHdr + self.payload )
		# Repack TCP header with correct checksum
		tcpHdr = struct.pack( "!HHLLBBH",
			self.srcPort,
			self.dstPort,
			self.seqn,
			self.ackn,
			data_offset,
			flags,
			self.window ) + \
			struct.pack( '<H', tcpChecksum ) + \
			struct.pack( '!H', self.urgp )
		return tcpHdr

	def createIPPacket( self ):
		"""
		Creates IP Packet with TCP payload defined by current object

		@return IPPacket object
		"""
		payloadIP = self.packHeader() + self.payload
		return IPPacket( self.srcHost, self.dstHost, socket.IPPROTO_TCP, payloadIP )


class UDPPacket( AbstractL4Packet ):
	""" Implemets UDP protocol encapsulation """

	def __init__( self, srcAddress, dstAddress, payload ):
		"""
		Initialises UDP segment

		@param srcAddress: Source address in form 'IP:port'
		@param dstAddress: Destination address in form 'IP:port'
		@param payload:    payload data to be included into UDP segment
		"""
		super( UDPPacket, self ).__init__( srcAddress, dstAddress )
		self.payload  = payload
		print("UDPPacket : f %s:%d to %s:%d" % (self.srcHost, self.srcPort, self.dstHost, self.dstPort))

	def packHeader( self ):
		"""
		Formats UDP header in a byte-order suitable for sending to socket

		@return formatted header as a Python string
		"""
		totalLen = 8 + len( self.payload )
		# Pack UDP header with zeroed checksum
		udpChecksum = 0
		udpHdr = struct.pack( '!HHHH',
			self.srcPort, self.dstPort, totalLen, udpChecksum )
		# Create pseudo header for checksum calculation
		pseudoHdr = struct.pack( "!4s4sBBH",
			socket.inet_aton( self.srcHost ),
			socket.inet_aton( self.dstHost ),
			0,					# reserved
			socket.IPPROTO_UDP, # protocol
			len( udpHdr ) + len( self.payload ) )
		pseudoHdr = pseudoHdr + udpHdr

		udpChecksum = checkSum( pseudoHdr + self.payload )
		# Pack UDP header with correct checksum
		udpHdr = struct.pack( '!HHH', self.srcPort, self.dstPort, totalLen ) + \
			struct.pack( '<H', udpChecksum )
		return udpHdr

	def createIPPacket( self ):
		"""
		Creates IP Packet with UDP payload defined by current object

		@return IPPacket object
		"""
		payloadIP = self.packHeader() + self.payload
		print("UDPPacket::createIPPacket: %s" % ( ' '.join('%02X' % ch for ch in payloadIP)))
		return IPPacket( self.srcHost, self.dstHost, socket.IPPROTO_UDP, payloadIP )


class ICMPPacket( AbstractL4Packet ):
	""" Implemets ICMP protocol encapsulation """

	# List of ICMP types
	ICMPTYPE_ECHO_REPLY		= 0
	ICMPTYPE_DEST_UNREACH 	= 3
	ICMPTYPE_SRC_QUENCH		= 4
	ICMPTYPE_REDIR_MSG		= 5
	ICMPTYPE_ECHO_REQUEST 	= 8
	ICMPTYPE_ROUTER_ADVT 	= 9
	ICMPTYPE_ROUTER_SOLIC 	= 10
	ICMPTYPE_TIME_EXCEED 	= 11
	ICMPTYPE_TRACEROUTE 	= 30

	# List of ICMP codes
	DEST_NET_UNREACH		= 0
	DEST_HOST_UNREACH		= 1
	DEST_PROT_UNREACH		= 2
	DEST_PORT_UNREACH		= 3
	FRAG_REQUIRED			= 4
	SRC_ROUTE_FAILED		= 5
	DEST_NET_UNKNOWN		= 6
	DST_HOST_UNKNOWN		= 7
	SRC_HOST_ISOLATED		= 8
	NET_ADM_PROHIBITED		= 9
	HOST_ADM_PROHIBITED		= 10
	NET_UNREACHABLE_TOS		= 11
	HOST_UNREACHABLE_TOS	= 12
	COM_ADM_PROHIBITED		= 13
	HOST_PRECEDENCE_VIOL	= 14	
	PRECEDENCE_CUTOFF		= 15

	def __init__( self, srcAddress, dstAddress, icmpType, icmpCode, payload ):
		"""
		Initialises ICMP packet

		@param srcAddress: Source address in form 'IP:port'
		@param dstAddress: Destination address in form 'IP:port'
		@param icmpType: Type of message. 
		@param icmpCode: Code of message. 
		@param payload:    payload data to be included into UDP segment
		"""
		super( ICMPPacket, self ).__init__( srcAddress, dstAddress )
		self.payload  = payload
		self.icmpType = icmpType
		self.icmpCode = icmpCode

	def packHeader( self ):
		"""
		Formats ICMP header in a byte-order suitable for sending to socket

		@return formatted header as a Python string
		"""
		# Create pseudo header for checksum calculation
		pseudoHdr = struct.pack( '!BBHL', self.icmpType, self.icmpCode, 0, 0 )
		icmpChecksum = checkSum( pseudoHdr + self.payloadICMP )
		# Pack ICMP header with correct checksum
		icmpHdr = struct.pack( '!BB', self.icmpType, self.icmpCode ) + \
			struct.pack( '<HL', icmpChecksum, 0 )	# checksum is little endian
		return icmpHdr

	def createIPPacket( self ):
		"""
		Creates IP Packet with ICMP payload defined by current object

		@return IPPacket object
		"""
		pktIncUDP = UDPPacket( 
			self.dstHost + ":" + str( self.dstPort ), 
			self.srcHost+":"+str( self.srcPort ), 
			self.payload )

		self.payloadICMP = pktIncUDP.createIPPacket().pack()
		payloadIP = self.packHeader() + self.payloadICMP
		return IPPacket( self.srcHost, self.dstHost, socket.IPPROTO_ICMP, payloadIP )


"""
Wrappers that run net functions without explicit object creation
"""

def injectUDP( srcAddress, dstAddress, data ):
	"""
	Build and inject UDP packet into network.

	@param srcAddress: Source address in form 'IP:port'
	@param dstAddress: Destination address in form 'IP:port'
	@param data: L5-L5 payload
	"""
	UDPPacket( srcAddress, dstAddress, bytearray.fromhex( data ) ).inject()

def injectTCP( srcAddress, dstAddress, data ):
	"""
	Build and inject TCP packet into network.

	@param srcAddress: Source address in form 'IP:port'
	@param dstAddress: Destination address in form 'IP:port'
	@param data: L5-L5 payload
	"""
	TCPPacket(  srcAddress, dstAddress, bytearray.fromhex( data ) ).inject()

def injectICMP( srcAddress, dstAddress, icmpType, icmpCode, data ):
	"""
	Build and inject ICMP packet into network.

	@param srcAddress: Source address in form 'IP:port'
	@param dstAddress: Destination address in form 'IP:port'
	@param icmpType: Type of message. Ex.: 3 is for 'Destination unreachable'
	@param icmpCode: Code of message. Ex.: 1 is for 'Destination host unreachable'
	@param data: L5-L7 layer data, that would be sent in original packet
	"""
	ICMPPacket( srcAddress, dstAddress, icmpType, icmpCode, bytearray.fromhex( data ) ).inject()
