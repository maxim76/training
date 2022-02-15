from net_tools import *

#netInject = net_tools2.NetInject()
#netInject.injectUDP( "10.175.33.146", 1234, "10.175.32.45", 4321, "00000000")
#netInject.injectUDP( "10.175.33.146:1234", "10.175.32.45:4321", bytearray.fromhex( "00000000" ) )

#ICMPPacket( "10.175.32.45:1234",  "10.175.33.146:7777", ICMPPacket.ICMPTYPE_DEST_UNREACH, ICMPPacket.DEST_HOST_UNREACH, bytearray.fromhex( "00000000" ) ).inject()
#ICMPPacket( "10.175.33.146:7777", "10.175.32.45:1234", ICMPPacket.ICMPTYPE_DEST_UNREACH, ICMPPacket.DEST_HOST_UNREACH, bytearray.fromhex( "01020304" ) ).inject()

#injectICMP( "10.175.47.207:7777", "10.175.32.45:1234", ICMPPacket.ICMPTYPE_DEST_UNREACH, ICMPPacket.DEST_HOST_UNREACH, "01020304" )
injectUDP( "10.175.47.207:1234", "10.175.32.45:4321", "0102030405060708" )
