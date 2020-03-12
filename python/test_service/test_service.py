#!/usr/bin/env python

# Import pycommon modules (always do this first!)
import bwsetup
bwsetup.addPath("..")

#from pycommon import bwmachined
#from pycommon import command_util
#from pycommon import uid
from pycommon import util

# Import standard modules
import time
import sys
import os
import logging
import signal
from optparse import OptionParser
from optparse import OptionGroup

# Logging module, set it up before importing local files
logging.basicConfig( format = '%(asctime)s: %(levelname)s  %(message)s' )
logging.getLogger().setLevel( logging.INFO )
log = logging.getLogger( __name__ )

# Import local files
from test_worker import Worker

#------------------------------------------------------------------------------
# Section: main function
#------------------------------------------------------------------------------
def main( args ):
	"""
	TestService main function.
	"""
	worker = Worker()
	# do worker configuration
	# like 	
	#	gatherer.addDataStore( dbStore )			
	
	# Start the worker
	worker.setDaemon( True )
	worker.start()

	# Enter thread monitor loop
	log.info( "Worker is working, press Ctrl-C to stop..." )
	try:
		while worker.isAlive():
			time.sleep( 1.0 )
		log.error( "Worker thread exited unexpectedly, aborting." )
		sys.exit( 1 )
	except KeyboardInterrupt:
		log.info( "KeyboardInterrupt received. Finishing." )
	
	# This may be raised when receiving SIGTERM signal ( in daemon mode)
	# We should catch this and then pass to stop gatherer part
	# Otherwise the gatherer will continues running
	except SystemExit:
		log.info( "SystemExit received. Finishing." )
	
	if worker.isAlive():
		#Telling Gather thread to finish up
		worker.pushFinish()

		# Wait for the Gather thread to finish up writing log
		log.info( "Waiting for Worker thread to complete logging" )

		# If there's an error which freezes, time out after a minute
		worker.join( 60.0 )

# main

def readArgs():
	"""
	Parse the command line arguments.
	"""
	#usage= "usage: %prog [options]"

	parser = OptionParser()

	parser.add_option("--pid", dest = "pidFile",
		metavar = "<pid_file>", default = "stat_logger.pid",
		help = "[daemon mode] Location to store PID file." )

	parser.add_option( "-o", "--output",
			default = None, dest = "stdoutFile",
			metavar = "<output_file>",
			help = "Log file to dump standard output (default is stdout)" )

	parser.add_option( "-e", "--erroutput",
			default = None, dest = "stderrFile",
			metavar = "<output_file>",
			help = "Log file to dump error output (default is stderr)" )

	parser.add_option( "-d", "--daemon",
			default = True, action = "store_false", dest = "foreground",
			help = "Run stat_logger in daemon mode " \
					"(default is to run in foreground)" )

	parser.add_option( "-v", "--verbose",
			default = False, action = "store_true", dest = "verbose",
			help = "Enable verbose debugging output" )

	parser.add_option( "-c", "--chdir",
		type = "string", dest = "chdir",
		help = "daemon working directory" )

	args, remain = parser.parse_args()

	return args

# readArgs


if __name__ == "__main__":
	# Read arguments
	args = readArgs()

	if args.foreground:
		main( args )
	else:
		# redirect log to files
		util.redirectLogOutputToFiles( args.stdoutFile, args.stderrFile )

		chdirPath = ""

		if args.chdir:
			chdirPath = args.chdir

		from pycommon.daemon import Daemon
		d = Daemon( run = main,
			args = (args,),
			workingDir = chdirPath,
			outFile = args.stdoutFile,
			errFile = args.stderrFile,
			pidFile = args.pidFile,
			umask = 0033
		)
		d.start()


# test_service.py
