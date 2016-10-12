'''
Created on 09.06.2013

@author: ko10ok
'''

from time import gmtime, strftime

class Logger(object):
    '''
    classdocs
    '''
    logger = None
    

    def __init__(self,logger=None):
        '''
        Constructor
        '''
        self.logger=logger
        
    def append(self, text):
        
        #%Y-%m-%d
        self.logger.appendPlainText(strftime("[%H:%M:%S] ", gmtime())+text)
    
    def save(self):
        #print(self.logger.toPlainText())
        file = open(strftime("logs/%Y-%m-%d_%H.%M.%S")+".log","w")
        file.write(self.logger.toPlainText())