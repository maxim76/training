#import operator
#import itertools
import threading
import time
import sys
import Queue
import logging
#import traceback
#import math

# Import pycommon modules
import bwsetup
bwsetup.addPath( ".." )
#from pycommon.watcher_data_message import WatcherDataMessage
#from pycommon import cluster
#from pycommon.exceptions import UserError
#from pycommon.stat_logger import constants


# Logging module
log = logging.getLogger( __name__ )


class Worker( threading.Thread ):
	""" Thread for doing Worker module stuff.  """
	FINISH = 0
	START_DEBUG = 1

	def __init__( self ):
		threading.Thread.__init__( self, name="Worker" )

		# Set to true if profiling StatGatherer thread
		self.profiling = False

		#  Message queue for handling messages from the main thread
		self.messageQueue = Queue.Queue()

	# __init__

	class FinishException(Exception):
		pass

	# ------------------------------------------------------
	# Section: exposed communication methods
	# ------------------------------------------------------
	def pushFinish( self, quick=False ):
		"""
		Send a finish signal to this thread.

		@param quick: True if we should stop immediately upon receiving
			the signal (does not write pending insert statements to the
			database)
		"""
		self.messageQueue.put( (self.FINISH, quick) )
	# pushFinish

	def pushDebug( self ):
		"""
		Send a signal to the thread to start the debugger
		(requires winpdb).
		"""
		self.messageQueue.put( (self.START_DEBUG,None) )
	# pushDebug


	def run( self ):
		""" Thread entry point """
		if self.profiling:
			import cProfile
			prof = cProfile.Profile()
			prof.runcall( self.runGather )
			prof.dump_stats( "worker.prof" )
			log.debug( "Dumped profile statistics to worker.prof" )
		else:
			self.runWorker()
		log.info( "Worker thread terminating normally" )
	# run


	def runWorker( self ):
		""" Handle the main Worker loop """				

		log.info( "Worker is ready." )

		try:
			# Start the main loop!
			while True:
				time.sleep(1)

				while not self.messageQueue.empty():
					self.processMessage( self.messageQueue.get(0) )

		except self.FinishException:
			pass

		self.finalise( self.quickTerminate )
	# runWorker


	def processMessage( self, messageTuple ):
		""" Handles messages passed to us from the main thread """
		msg, params = messageTuple
		if msg == self.START_DEBUG:
			import rpdb2
			pauseTime = 15
			log.info( "Waiting %ds for debugger to connect", pauseTime )
			rpdb2.start_embedded_debugger(
				"abcd",
				fAllowUnencrypted = True,
				fAllowRemote = True,
				timeout = pauseTime,
				fDebug = False )
		elif msg == self.FINISH:
			log.info( "Stopping Worker" )
			self.quickTerminate = params
			raise self.FinishException()
	# processMessage

	def finalise( self, quickTerminate = True ):
		pass
	# finalise

# Worker.py
