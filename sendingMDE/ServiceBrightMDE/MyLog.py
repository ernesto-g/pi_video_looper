import logging
import logging.handlers

logging.basicConfig(level=logging.DEBUG) # change logger level here
	
class MyLog:
	logger = None

	@staticmethod
	def getLogger(pathToLogger="."):
		if MyLog.logger==None:
			l = logging.getLogger("BrightService")
			fh = logging.handlers.RotatingFileHandler(pathToLogger+'/brightService.log',maxBytes=512*1024, backupCount=5)
			formatter = logging.Formatter('%(asctime)s [%(levelname)s] %(message)s')
			fh.setLevel(logging.DEBUG)
			fh.setFormatter(formatter)
			l.addHandler(fh)
			MyLog.logger = l
		
		return MyLog.logger
