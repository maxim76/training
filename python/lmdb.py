"""
Imitate light-weight in-memory key-value database
"""

class LMDB( dict ):
	def __init__( self, fileName ):
		self.fileName = fileName
		try:
			fHandler = open( self.fileName, 'rb' )
			import pickle
			self.update( pickle.load( fHandler ) )
			#print( 'Load successfull: %s' % self.copy() )
			fHandler.close()
		except IOError:
			pass
		except Exception as E:
			raise E

	def sync( self ):
		fHandler = open( self.fileName, 'wb' )
		import pickle
		pickle.dump( self.copy(), fHandler )
		fHandler.close()

	def __setitem__( self, key, value ):
		super( LMDB, self ).__setitem__( key, value)
		self.sync()


if __name__ == "__main__":
	import time
	db = LMDB( 'lmdb.db' )
	print( "Before" )
	print( db )
	db[ 'key_('+time.asctime()+')' ] = 'value_('+time.asctime()+')'
	print( "After" )
	print( db )
