#!/usr/bin/python

import threading
import time
import socket

exitFlag = 0

class myThread (threading.Thread):
	def __init__(self, threadID, name, counter):
		threading.Thread.__init__(self)
		self.threadID = threadID
		self.name = name
		self.counter = counter
		self.stopped = False
		self.socket_ = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		self.socket_.bind(('', 0))
		self.socket_.settimeout(1)


	def run(self):
		print "Thread : Starting " + self.name
		while not self.stopped:
			time.sleep(2)
			print "Thread : %s: %s  read from socket" % (self.name, time.ctime(time.time()))
			try:
				data, addr = self.socket_.recvfrom( 1000 )
			except socket.timeout:
				continue
			except:
				raise
			print "Thread : socket read done"

		print "Thread : Exiting " + self.name
		self.socket_.close()

# Create new threads
thread1 = myThread(1, "Thread-1", 1)
#thread2 = myThread(2, "Thread-2", 2)

# Start new Threads
thread1.start()
#thread2.start()

#print "Exiting Main Thread"
print "Main: thread started. Wait 1 sec"
time.sleep(10)
print "Main: Stopping thread"
thread1.stopped = True
#thread1.socket_.shutdown( socket.SHUT_RDWR )
#thread1.socket_.close()
print "Main: joining..."
thread1.join()

print "Main Thread joined thread1"
