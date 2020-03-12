from urllib.request import urlopen
reply = urlopen('http://localhost/cgi-bin/cgi101.py?user=Bob').read()
print(reply)
