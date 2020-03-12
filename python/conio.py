#!/usr/bin/python
from __future__ import print_function
import sys
import os

# if Linux
if os.name == 'posix':

	class Colors:
		Black	= '\033[30m'
		Red		= '\033[31m'
		Green	= '\033[32m'
		Yellow	= '\033[33m'
		Blue	= '\033[34m'
		Magenta = '\033[35m'
		Cyan	= '\033[36m'
		LGrey	= '\033[37m'
		Grey	= '\033[90m'
		LRed	= '\033[91m'
		LGreen	= '\033[92m'
		LYellow	= '\033[93m'
		LBlue	= '\033[94m'
		LMagenta = '\033[95m'
		LCyan	= '\033[96m'
		White	= '\033[97m'
		Normal = '\033[0m'

		Header	= White
		Info	= LBlue
		Warning	= LYellow
		Error	= LRed
		OK		= Green

	def cprintf(color, str, *args):
		#global USER_DEBUG				# USER_DEBUG should be controlled by '-u' parameter of ./runtest
		#if USER_DEBUG == 1:
		print(color + (str % args) + Colors.Normal, end='')
		sys.stdout.flush()

# if Windows
else:
	from ctypes import windll

	STD_OUTPUT_HANDLE = -11
	stdout_handle = windll.kernel32.GetStdHandle(STD_OUTPUT_HANDLE)
	SetConsoleTextAttribute = windll.kernel32.SetConsoleTextAttribute

	class Colors:
		Black	= 0x0000
		Blue	= 0x0001
		Green	= 0x0002
		Cyan	= 0x0003
		Red		= 0x0004
		Magenta = 0x0005
		Yellow	= 0x0006
		Grey	= Black    | 0x0008
		LGrey	= 0x0007
		LBlue	= Blue     | 0x0008
		LGreen	= Green    | 0x0008
		LCyan	= Cyan     | 0x0008
		LRed	= Red      | 0x0008
		LMagenta = Magenta | 0x0008
		LYellow	= Yellow   | 0x0008
		White	= LGrey    | 0x0008

		Normal  = LGrey
		Header	= White
		Info	= LBlue
		Warning	= LYellow
		Error	= LRed
		OK		= Green

	def cprintf(color, str, *args):
		#global USER_DEBUG				# USER_DEBUG should be controlled by '-u' parameter of ./runtest
		#if USER_DEBUG == 1:
		SetConsoleTextAttribute( stdout_handle, color )
		print( (str % args), end='')
		SetConsoleTextAttribute( stdout_handle, Colors.Normal )
		sys.stdout.flush()

# end Linux/Windows platform-specific code

def printf(str, *args):
	#global USER_DEBUG				# USER_DEBUG should be controlled by '-u' parameter of ./runtest
	#if USER_DEBUG == 1:
	print( str % args, end='')
	sys.stdout.flush()
