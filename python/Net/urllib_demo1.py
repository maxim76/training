import sys
from urllib.request import urlopen
showlines = 6
try:
	servername, filename = sys.argv[1:] # cmdline args?
except:
	servername, filename = 'example.com', '/index.html'

remoteaddr = 'http://%s%s' % (servername, filename) # can name a CGI script too
print(remoteaddr)
remotefile = urlopen(remoteaddr) # returns input file object
remotedata = remotefile.readlines() # read data directly here
remotefile.close()
for line in remotedata[:showlines]: print(line) # bytes with embedded \n


# Test 2
import urllib.request
#urllib.request.urlretrieve(remoteaddr, "file.dat")
urllib.request.urlretrieve('file://C:/Work/PremiumSpectate/premspec-clientcontrol/unittest_res/testshort.wotsrvreplay', 'G:/Despair/recordings/unittest/download/testing.wotsrvreplay')
