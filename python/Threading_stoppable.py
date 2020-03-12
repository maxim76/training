#!/usr/bin/python

import threading
import time

exitFlag = 0

class myThread (threading.Thread):
	def __init__(self, threadID, name, counter):
		threading.Thread.__init__(self)
		self.threadID = threadID
		self.name = name
		self.counter = counter
		self.stopped = False

	def run(self):
		print( "Starting " + self.name )
		while not self.stopped:
			time.sleep(1)
			print( "%s: %s" % (self.name, time.ctime(time.time())) )

		print( "Exiting " + self.name )


# Create new threads
thread1 = myThread(1, "Thread-1", 1)
#thread2 = myThread(2, "Thread-2", 2)

# Start new Threads
thread1.start()
#thread2.start()

#print "Exiting Main Thread"
time.sleep(4)
thread1.stopped = True

thread1.join()
print( "Main Thread joined thread1" )
