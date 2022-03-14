import zmq
import random
import time


context = zmq.Context()

# Socket to send messages on
sender = context.socket(zmq.PUSH)
sender.bind("tcp://*:5557")

# Socket to receive messages on
receiver = context.socket(zmq.PULL)
receiver.bind("tcp://*:5558")

print("Press Enter when the workers are ready: ")
_ = input()
print("Sending tasks to workers...")

while True:
    try:
        sender.send(b"Hello", zmq.NOBLOCK)
        print("Sent")
    except:
        print("Failed to send")

    try:
        s = receiver.recv(zmq.NOBLOCK)
        print("Received something")
    except:
        print("Failed to receive")

    time.sleep(1)

