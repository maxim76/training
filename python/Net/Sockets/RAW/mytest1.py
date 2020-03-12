import socket
import os

print('socket.socket()')
s=socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_UDP)

print('bind()')
# Get the current network interface
host=socket.gethostbyname(socket.gethostname())
s.bind((host, 0))

#print 'Enable promiscuous mode'
# Enable promiscuous mode
#s.ioctl(socket.SIO_RCVALL, socket.RCVALL_ON) 	# this for Windows
#this for linux
#ret =  os.system("ifconfig enp0s3 promisc")
#if ret != 0:
#	print 'Error: cannot enable promiscuous mode'
#	exit

print 'shiff packet'
#while True:
packet=s.recvfrom(65565)

# change the socket option in order to inject Raw packets
s.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)

# send manually constructed packet
s.sendto(ipheader+data)

print('Done')
