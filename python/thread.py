import thread
import time

# Define a function for the thread
def print_time( threadName, delay):
	count = 0
	while count < 5:
		time.sleep(delay)
		count += 1
		print( "%s: %s" % ( threadName, time.ctime(time.time()) ))

	print( "%s completed" % (threadName))

# Create two threads as follows
try:
	t1 = thread.start_new_thread( print_time, ("Thread-1", 2, ) )
	t2 = thread.start_new_thread( print_time, ("Thread-2", 4, ) )
except:
	print( "Error: unable to start thread" )

while 1:
	pass
