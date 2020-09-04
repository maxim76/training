import logging
import logging.config
import datetime

dictLogConfig = {
        "version":1,
        "handlers":{
            "fileHandler":{
                "class":"logging.FileHandler",
                "level":"DEBUG",
                "formatter":"logFormatter",
                "filename":"log_" + datetime.datetime.now().strftime('%Y%m%d_%H%M%S.log')
            },
            "consoleHandler":{
                "class":"logging.StreamHandler",
                "level":"DEBUG",
                "formatter":"logFormatter"
            }
        },
        "loggers":{
            "":{        # empty logger name is root. Include everything, starting from root, to allow log capture from 3rd party libs, that uses root logger
                "handlers":["fileHandler", "consoleHandler"],
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

logging.debug('debug message')
logging.info('info message')
logging.warning('warn message')
logging.error('error message')
logging.critical('critical message')
