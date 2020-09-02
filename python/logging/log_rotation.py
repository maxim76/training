import logging
import logging.config
import logging.handlers

# Dict based config
dictLogConfig = {
        "version":1,
        "handlers":{
            "rotatingFileHandler":{
                "class":"logging.handlers.TimedRotatingFileHandler",
                "level":"DEBUG",
                "formatter":"logFormatter",
                "filename":"/rotatingLog",
                "when":"S",
                "interval":10
            },
            "consoleHandler":{
                "class":"logging.StreamHandler",
                "level":"DEBUG",
                "formatter":"logFormatter"
            }
        },
        "loggers":{
            "root":{        # empty logger name is root. Include everything, starting from root, to allow log capture from 3rd party libs, that uses root logger
                "handlers":["consoleHandler","rotatingFileHandler"],
                "level":"DEBUG",
            },
        },
        "formatters":{
            "logFormatter":{
                "format":"%(asctime)s %(levelname)-8s %(name)-24s %(message)s"
            }
        }
}

logging.config.dictConfig(dictLogConfig)
logger = logging.getLogger("root")

'''
# Configuring using separate logging functions

logger.setLevel(logging.DEBUG)

# Configure console log handler
consoleHandler = logging.StreamHandler()
consoleHandler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s %(levelname)-8s %(name)-24s %(message)s', datefmt='%Y/%m/%d %H:%M:%S')
consoleHandler.setFormatter(formatter)
logger.addHandler(consoleHandler)

# Configure file log handler (with rotation)
fileHandler = logging.handlers.TimedRotatingFileHandler("rotatingLog.log", when="S", interval=10)
fileHandler.setLevel('DEBUG')
fileHandler.setFormatter(formatter)
fileHandler.suffix = "%Y.%m.%d_%H-%M-%S.log"
logger.addHandler(fileHandler)
'''

import time
while True:
    logger.info("log message")
    time.sleep(1)
