'''
How matplotlib.pyplot.imshow draws
Default befavior, when no option specified is to draw is is. 
I.e. the first byte if file is left topmost pixel. Last byte in file is reght bottom most.
Byte order is: topmost line, second line,... bottom line.

Attention: specifying option for imshow reverts/mirrors image

I did not find a way to leafe image as is but change labels as required (for ex, make (0,0) to be in the center and make Y coord to increase from bottom to top
'''

import sys
import matplotlib.pyplot as plt
import numpy
import struct

dimX = 16
dimY = 8
if (len(sys.argv) < 2):
	filename = "test_16x8_lines.bin"
else:
	filename = sys.argv[1]
a = numpy.zeros((dimY, dimX))
f=open(filename,'rb')
for y in range(dimY):
	for x in range(dimX):
		a[y,x] = struct.unpack('B', f.read(1))[0]

f.close()

plt.imshow(a)
#plt.imshow(a, origin='lower', extent=(0,15,0,7))
#plt.xlim(0, dimX-1)
#plt.ylim(0, dimY-1)
#plt.ylim(bottom=0,top=dimY)
#print(plt.ylim())
#plt.ylim(0,7)
plt.show()
