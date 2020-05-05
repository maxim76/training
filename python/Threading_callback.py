from threading import Thread

class MyThread(Thread):
    def __init__(self, name, callback):
        Thread.__init__(self)
        self.name = name
        self.callback = callback

    def run(self):
        import time
        print("MyThread:run started")
        time.sleep(5)
        print("MyThread:run stopped")
        self.callback(self.name)

def onThreadDone(name):
    print("onThreadDone() : Thread %s is completed" % name)

import time
t = MyThread("Test thread", onThreadDone)
print("main() : Starting thread")
t.start()

counter=0
print("main() : Starting main prog cycle")

while(counter<10):
	print("main() : counter = %d" % counter)
	time.sleep(1)
	counter += 1

print("main() : Stop main prog cycle")
print("main() : joining thread (should already be stopped)")
t.join()
print("main() : All done")

