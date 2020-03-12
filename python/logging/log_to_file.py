import logging

"""
All 3 messagies will come ti file (becasue file loglevel is INFO)
"""
logging.basicConfig(filename='example.log',level=logging.DEBUG)
logging.debug('This message should go to the log file')
logging.info('So should this')
logging.warning('And this, too')
