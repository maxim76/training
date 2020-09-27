import logging
import datetime
from logging.handlers import TimedRotatingFileHandler
logger = logging.getLogger()
logger.setLevel(logging.DEBUG)

formatter = logging.Formatter('%(asctime)s %(levelname)-8s %(name)-24s %(message)s', datefmt='%Y/%m/%d %H:%M:%S')

# Configure console log handler
consoleHandler = logging.StreamHandler()
consoleHandler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s %(levelname)-8s %(name)-24s %(message)s', datefmt='%Y/%m/%d %H:%M:%S')
consoleHandler.setFormatter(formatter)
logger.addHandler(consoleHandler)

# Configure file log handler (with rotation)
def namer(name):
    logFileName = datetime.datetime.now().strftime('%Y%m%d_%H%M%S.log')
    return logFileName

fileHandler = TimedRotatingFileHandler('log.txt', when='S', interval=5)
fileHandler.setLevel('DEBUG')
fileHandler.setFormatter(formatter)
fileHandler.namer = namer
logger.addHandler(fileHandler)

import time
while(True):
    logger.info("Test log")
    time.sleep(1)
    