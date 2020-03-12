import time
from threading import Thread

class MyThread(Thread):
	def __init__(self, activity, callback):
		self.callback = callback
		Thread.__init__(self, target=activity)

	def update(self):
		if not self.is_alive():
			self.callback()


def func():
	print("Func started long running process")
	time.sleep(5)
	print("Func stopped long running process")


def callback():
	print("Callback: thread completed its activity")


myThread = MyThread(func, callback)
myThread.start()

counter=0
while(counter<10):
	print("Main: counter = %d" % counter)
	myThread.update()
	time.sleep(1)
	counter += 1

#myThread.join()
