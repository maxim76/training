from __future__ import print_function

f = open('file.txt','w')
f.write("Test file optput 1\n")
f.write("Test file optput 2\n")
f.write("Test file optput 3\n")
f.write("Test file optput 4\n")
f.write("Test file optput 5\n")
f.close()

f = open('file.txt','r+')
f.write("Write to head\n")
f.seek(0, 2)
f.write("Write to tail\n")
f.close()

print( "=====Read all file into single buffer:=====" )
f = open('file.txt','r')
data = f.read()
f.close()
print( data )

print( "\n=====Read all file into single buffer and split into lines:=====" )
f = open('file.txt','r')
lines = f.read().splitlines()
f.close()
for line in lines:
	print( "Line: %s" % line )

print( "\n=====Read all file into list of lines:=====" )
f = open('file.txt','r')
lines = f.readlines()
f.close()
for line in lines:
	print( "Line: %s" % line, end = '' )

print( "\n=====Read file line by line:=====" )
f = open('file.txt','r')
line = f.readline()
while ( line ):
	print( "Line: %s" % line, end='' )
	line = f.readline()
f.close()

print( "\n=====Read file using iterator (python recommended way):=====" )
for line in open('file.txt','r'):
	print( line, end='' )
