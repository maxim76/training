import logging
#logging.basicConfig(format='%(asctime)s %(message)s')
logging.basicConfig(format='%(asctime)s %(message)s', datefmt='%m/%d/%Y %I:%M:%S %p')
logging.debug( "Test DEBUG message" )
logging.info( "Test INFO message" )
logging.warning( "Test WARNING message" )
