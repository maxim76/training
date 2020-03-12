"""
Server side: open a socket on a port, listen for a message from a client,
and send an echo reply; forks a process to handle each client connection;
child processes share parent's socket descriptors; fork is less portable
than threads--not yet on Windows, unless Cygwin or similar installed;
"""
from __future__ import print_function
import os, time, sys, signal
from socket import * # get socket constructor and constants

myHost = '' # server machine, '' means local host
myPort = 50007 # listen on a non-reserved port number

sockobj = socket(AF_INET, SOCK_STREAM) # make a TCP socket object
sockobj.bind((myHost, myPort)) # bind it to server port number
sockobj.listen(5) # allow 5 pending connects
signal.signal(signal.SIGCHLD, signal.SIG_IGN) # avoid child zombie processes

def now(): # current time on server
	return time.ctime(time.time())

def handleClient(connection): # child process: reply, exit
	time.sleep(5) # simulate a blocking activity
	while True: # read, write a client socket
		data = connection.recv(1024) # till eof when socket closed
		if not data: break
		reply = 'Echo=>%s at %s' % (data, now())
		connection.send(reply.encode())
	connection.close()
	os._exit(0)

def dispatcher(): # listen until process killed
	while True: # wait for next connection,
		connection, address = sockobj.accept() # pass to process for service
		print('Server connected by', address, end=' ')
		print('at', now())
		childPid = os.fork() # copy this process
		if childPid == 0: # if in child process: handle
			handleClient(connection)

dispatcher()
