import sys

print "Lists passed parameter. Usage: params.py Param1 PAram2 Param3 ... "
print "argv = [" + str(sys.argv) + "]"
for i in range(len(sys.argv)):
	print "Param[%d] = [%s]" % (i, sys.argv[i])
