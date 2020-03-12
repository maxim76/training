"""
#############################################################################
implement client and server-side logic to transfer an arbitrary file from
server to client over a socket; uses a simple control-info protocol rather
than separate sockets for control and data (as in ftp), dispatches each
client request to a handler thread, and loops to transfer the entire file
by blocks; see ftplib examples for a higher-level transport scheme;
#############################################################################
"""

import sys, os, time, _thread as thread
#import sys, os, time, thread
from socket import *

blksz = 1024
defaultHost = 'localhost'
defaultPort = 50001

helptext = """
Usage...
server=> getfile.py -mode server [-port nnn] [-host hhh|localhost]
client=> getfile.py [-mode client] -file fff [-port nnn] [-host hhh|localhost]
"""

def now():
	return time.asctime()

def parsecommandline():
	dict = {} # put in dictionary for easy lookup
	args = sys.argv[1:] # skip program name at front of args
	while len(args) >= 2: # example: dict['-mode'] = 'server'
		dict[args[0]] = args[1]
		args = args[2:]
	return dict

def client(host, port, filename):
	sock = socket(AF_INET, SOCK_STREAM)
	sock.connect((host, port))
	sock.send((filename + '\n').encode())	# send remote name with dir: bytes
	dropdir = os.path.split(filename)[1]	# filename at end of dir path
	file = open(dropdir, 'wb')				# create local file in cwd
	while True:
		data = sock.recv(blksz) # get up to 1K at a time
		if not data: break # till closed on server side
		file.write(data) # store data in local file
	sock.close()
	file.close()
	print('Client got', filename, 'at', now())

def serverthread(clientsock):
	sockfile = clientsock.makefile('r') # wrap socket in dup file obj
	filename = sockfile.readline()[:-1] # get filename up to end-line
	"""
	try:
		file = open(filename, 'rb')
		while True:
			bytes = file.read(blksz) # read/send 1K at a time
			if not bytes: break # until file totally sent
			sent = clientsock.send(bytes)
			assert sent == len(bytes)
	except:
		print('Error downloading file on server:', filename)
	"""
	file = open(filename, 'rb')
	while True:
		bytes = file.read(blksz) # read/send 1K at a time
		if not bytes: break # until file totally sent
		sent = clientsock.send(bytes)
		assert sent == len(bytes)
	clientsock.close()

def server(host, port):
	serversock = socket(AF_INET, SOCK_STREAM) # listen on TCP/IP socket
	serversock.bind((host, port)) # serve clients in threads
	serversock.listen(5)
	while True:
		clientsock, clientaddr = serversock.accept()
		print('Server connected by', clientaddr, 'at', now())
		thread.start_new_thread(serverthread, (clientsock,))

def main(args):
	host = args.get('-host', defaultHost) 		# use args or defaults
	port = int(args.get('-port', defaultPort)) 	# is a string in argv
	if args.get('-mode') == 'server': 			# None if no -mode: client
		if host == 'localhost': host = '' 		# else fails remotely
		server(host, port)
	elif args.get('-file'): # client mode needs -file
		client(host, port, args['-file'])
	else:
		print(helptext)

if __name__ == '__main__':
	args = parsecommandline()
	main(args)
