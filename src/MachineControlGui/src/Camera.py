'''
Created on 26.10.2013

@author: SCB
'''
from src.Commutatorv2 import Commutatorv2

class Camera(object):
    #def values
    camDirectionAngle=0
    keylist = []

    updater = None
    
    cam = Commutatorv2("192.168.2.105", 80, False)
    
    
    start = "GET /cgi-bin/encoder?USER="
    login = "admin"
    pwd = "123456"
    end = " HTTP/1.1\r\nUser-Agent: EPLCommand\r\nHost: 192.168.2.105\r\nDNT: 1\r\nCache-Control: no-cache\r\n\r\n"

    hdirection=0
    vdirection=0
    zdirection=0    
    
    def __init__(self, argv):
        '''
        '''
    
    def move(self):
        '''
        '''
        print(self.hdirection,self.vdirection)
        
        if (self.hdirection != 0 and self.vdirection != 0):
            '''
            '''
        else:
            if (self.hdirection != 0 or self.vdirection != 0):
                if (self.hdirection != 0):
                    '''
                    '''
                    if (self.hdirection == 1):
                        self.moveLeft()
                    else:
                        self.moveRight()
                
                if (self.vdirection != 0):
                    '''
                    '''
                    if (self.vdirection == 1):
                        self.moveUp()
                    else:
                        self.moveDown()                    
            else:
                self.moveStop()
            
        if (self.zdirection != 0):
            '''
            '''
            if (self.zdirection == 1):
                self.zoomTele()
            else:
                self.zoomWide()       
        else:
            self.zoomStop()
        
    
    def moveLeft(self):
        login = "admin"
        pwd = "123456"
        dir = "LEFT"
        speed = "5"
        end = " HTTP/1.1\r\nUser-Agent: EPLCommand\r\nHost: 192.168.2.105\r\nDNT: 1\r\nCache-Control: no-cache\r\n\r\n"
        str="GET /cgi-bin/encoder?USER="+login+"&PWD="+pwd+"&MOVE="+dir+","+speed+end


        str= self.start+self.login+"&PWD="+self.pwd+"&MOVE="+dir+","+speed+self.end
        self.cam.getPage(str)
        '''      
        str="GET /cgi-bin/encoder?USER=admin&PWD=123456&MOVE=LEFT,3 HTTP/1.1\r\n"
        str="GET /cgi-bin/encoder?USER=admin&PWD=123456&MOVE=UP,3 HTTP/1.1\r\n"
        str="GET /cgi-bin/encoder?USER=admin&PWD=123456&MOVE=DOWN,3 HTTP/1.1\r\n"
        str="GET /cgi-bin/encoder?USER=admin&PWD=123456&MOVE=UPRIGHT,3,3 HTTP/1.1\r\n"
        str="GET /cgi-bin/encoder?USER=admin&PWD=123456&MOVE=DOWNLEFT,3,3 HTTP/1.1\r\n"
        str="GET /cgi-bin/encoder?USER=admin&PWD=123456&MOVE=UPLEFT,3,3 HTTP/1.1\r\n"
        str="GET /cgi-bin/encoder?USER=admin&PWD=123456&MOVE=DOWNRIGHT,3,3 HTTP/1.1\r\n"
        str="GET /cgi-bin/encoder?USER=admin&PWD=123456&ZOOM=TELE,4 HTTP/1.1\r\n"
        str="GET /cgi-bin/encoder?USER=admin&PWD=123456&ZOOM=WIDE,4 HTTP/1.1\r\n"
        str="GET /cgi-bin/encoder?USER=admin&PWD=123456&MOVE=STOP,3,3 HTTP/1.1\r\n"
        str="GET /cgi-bin/encoder?USER=admin&PWD=123456&ZOOM=STOP,4 HTTP/1.1\r\n"
        '''
    
    def moveReset(self):
        dir = "RESET"
        str= self.start+self.login+"&PWD="+self.pwd+"&MOVE="+dir+","+self.end

        self.cam.getPage(str)
        
    def moveRight(self):
        dir = "RIGHT"
        speed = "5"
        str= self.start+self.login+"&PWD="+self.pwd+"&MOVE="+dir+","+speed+self.end

        self.cam.getPage(str)

    def moveUp(self):
        dir = "UP"
        speed = "5"
        str= self.start+self.login+"&PWD="+self.pwd+"&MOVE="+dir+","+speed+self.end

        self.cam.getPage(str)
    
    def moveDown(self):

        dir = "DOWN"
        speed = "5"
        str= self.start+self.login+"&PWD="+self.pwd+"&MOVE="+dir+","+speed+self.end

        self.cam.getPage(str)

    def moveStop(self):
        
        dir = "STOP"
        speed = "5,5"
        str= self.start+self.login+"&PWD="+self.pwd+"&MOVE="+dir+","+speed+self.end

        self.cam.getPage(str)

    def zoomTele(self):
        
        dir = "TELE"
        speed = "5"
        str= self.start+self.login+"&PWD="+self.pwd+"&ZOOM="+dir+","+speed+self.end

        self.cam.getPage(str)
        
        
    def zoomWide(self):
        
        dir = "WIDE"
        speed = "5"
        str= self.start+self.login+"&PWD="+self.pwd+"&ZOOM="+dir+","+speed+self.end

        self.cam.getPage(str)
        
    def zoomStop(self):
        
        dir = "STOP"
        speed = "5"
        str= self.start+self.login+"&PWD="+self.pwd+"&ZOOM="+dir+","+speed+self.end

        self.cam.getPage(str)
        
    def home(self):
       
        str= self.start+self.login+"&PWD="+self.pwd+"&POSITION=ABSOLUTE,850,0,5,5"+self.end
        
        self.cam.getPage(str)        
        
        