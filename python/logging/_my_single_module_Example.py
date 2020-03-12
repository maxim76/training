"""
Start me.
Suitable fo single file only, because does not suppoty module name(s) in logs
"""

import logging

logging.basicConfig(format='%(asctime)s [%(levelname)-8s] %(message)s', datefmt='%Y/%m/%d %H:%M:%S', level=logging.DEBUG)

logging.debug( "Test DEBUG message" )
logging.info( "Test INFO message" )
logging.warning( "Test WARNING message" )
