"""
Only wraning will be printed to console becasue default loglevel is warning
"""
import logging

logger = logging.getLogger( __name__ )
logger.setLevel( logging.INFO )

logger.debug( "Test DEBUG message" )
logger.info( "Test INFO message" )
logger.warning( "Test WARNING message" )
