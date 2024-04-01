start "MessageLogger1" py -3 pub_sub_recv.py 5555 5556 5557
start "MessageLogger2" py -3 pub_sub_recv.py 5555 5556 5557
start "App1" py -3 pub_sub_send.py 5555
start "App2" py -3 pub_sub_send.py 5556
start "App3" py -3 pub_sub_send.py 5557
