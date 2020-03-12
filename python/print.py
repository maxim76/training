#!/usr/bin/python

from __future__ import print_function

def printf(str, *args):
    print(str % args, end='')

printf("Start\n")

for x in xrange(10):
    printf('.')

printf("\nStop\n")

printf("Str = %-20s X = %d\n", "Ckgvmtrbv", 13)
printf("Str = %-20s X = %d\n", "Ckgvmtrbvrvrvrvv", 136)
printf("Str = %-20s X = %d\n", "Ckgvmt", 13)


import sys
for i in xrange(0,10):
   sys.stdout.write(".")
   sys.stdout.flush()
