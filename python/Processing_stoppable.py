from multiprocessing import Process, Lock
import time

class MyProcess(Process):
	def __init__(self):
		Process.__init__(self)

	def run(self):
		while(True):
			time.sleep(1)
			print("Process works")

if __name__ == '__main__':
# ATTENTION:  
# if __name__ == '__main__': must be presented to avoid Runtime error  freeze_support()
	print("Main: started")
	p = MyProcess()
	p.start()
	print("Main: process started")
	time.sleep(3)
	print("Main: killing process")
	p.kill()
	time.sleep(3)
	print("Main: exiting")

"""
def task(lock):
	while(True):
		time.sleep(1)
		with lock:
			print("Process works")

if __name__ == '__main__':
# ATTENTION:  
# if __name__ == '__main__': must be presented to avoid Runtime error  freeze_support()
	lock = Lock()
	with lock:
		print("Main")
	p = Process(target=task, args = (lock,))
	p.start()
	time.sleep(5)
	p.terminate()
	p.join()
"""