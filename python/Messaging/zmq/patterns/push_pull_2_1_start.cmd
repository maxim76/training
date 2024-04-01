start "Send 1" py -3 push_pull_send.py 5555
start "Send 2" py -3 push_pull_send.py 5555
start "Recv" py -3 push_pull_recv.py 5555