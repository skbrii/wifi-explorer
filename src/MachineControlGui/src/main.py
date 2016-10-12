# -*- coding: utf-8 -*-

'''
Created on 01.06.2013

@author: ko10ok
'''
import sys
sys.path.insert(0, '..')
from PyQt4 import QtGui, QtCore
#from  import Ui_MainWindow,_fromUtf8
from layout.mainui import Ui_MainWindow,_fromUtf8
from src.Logger import Logger
from src.EthernetDevice import EthernetDevice
from PyQt4.phonon import Phonon
from src.Camera import Camera

import threading
import time

class DataUpdateThread(QtCore.QThread):
    trigger = QtCore.pyqtSignal(int)
    upd = True

    def __init__(self, parent=None):
        super(DataUpdateThread, self).__init__(parent)

    def stop(self):
        self.upd = False

    def run(self): #should be modified to event logic
        while self.upd:
            time.sleep(0.1)  #random sleep to imitate working
            self.trigger.emit(2) 

class ControlApp(QtGui.QMainWindow, Ui_MainWindow):
    #def values
    camDirectionAngle=0
    keylist = []
    cam = None
    updater = None
    def __init__(self, argv):
        QtGui.QMainWindow.__init__(self)
        self.MainWindow=Ui_MainWindow
        self.ui=Ui_MainWindow
        self.MainWindow.__init__(self.window)
        self.setupUi(self)
        self.show()
        
        self.logger = Logger(self.logPanel)
        self.device = EthernetDevice(self.logger)
        
        self.device.disconState = QtGui.QPixmap(_fromUtf8('../res/disconnected'))
        self.device.conState = QtGui.QPixmap(_fromUtf8('../res/connected'))
        self.device.currentState = 0
        
        self.device.power=0
        self.device.direction=0
        
        self.addWidgets()
        
        self.updater = DataUpdateThread(self)    # create a thread
        self.updater.trigger.connect(self.updateState)  # connect to it's signal
        self.updater.start() 
        
        self.cam = Camera(self)
        
    def addWidgets(self):
        """ В этом методе мы добавляем виджеты и wприсоединяем обработчики сигналов.
            Обработчик сигнала для виджета так же называется "слотом"
        """
        
        #camview setup
        #path="C:\\Users\\SCB\\Downloads\\test.avi"
        
        path=QtCore.QUrl("rtsp://192.168.2.100:7070/")
        media = Phonon.MediaSource(path)
        self.videoPlayer.load(media)
        self.videoPlayer.play()
        
        #set maps properties
        url = "res.gmap.html"
        page = QtCore.QUrl("../res/gmap.html")
        self.mapView.load(page)
        
        #thumper pic
        myPixmap = QtGui.QPixmap(_fromUtf8('../res/thumper_top.jpg'))
        self.thumperImage.setPixmap(myPixmap)
        self.thumperImage.setScaledContents(True)
        
        #camDirection
        myPixmap = QtGui.QPixmap(_fromUtf8('../res/cam.png'))
        myPixmap = myPixmap.transformed(QtGui.QTransform().rotate(self.camDirectionAngle))
        self.camDirection.setPixmap(myPixmap)
        self.camDirection.setScaledContents(True)
                
        #logger 
        self.actionSaveLog.triggered.connect(self.saveLog)
        self.actionSaveLog.setShortcut('Ctrl+S')
        
        #define menu actions
        self.actionExit.triggered.connect(self.exitApp)
        self.actionExit.setShortcut('Ctrl+Q')
        
        #device state
        #self.connectStateIndicator.connect(self.connectStateIndicator, QtCore.SIGNAL("")
        self.connectStateIndicator.setPixmap(self.device.disconState)
        self.connectStateIndicator.setScaledContents(True)
        self.actionConnect.triggered.connect(self.changeConnectionState)
        self.actionConnect.setShortcut('F5')
        self.resetTelemetry()
        
        #get state
        self.actionBattery.triggered.connect(self.addMarkerWithLabel)
        
        #init direction level (altitude direction indicator)
        
        #init battery state
        #self.actionBattery.setShortcut('R')  
        
    def keyPressEvent(self, event):
        
        if event.key() not in self.keylist:
            self.keylist.append(event.key())
            self.processmultikeys(self.keylist)

    def keyReleaseEvent(self, event):
        
        if event.key() in self.keylist:
            if not event.isAutoRepeat():
                self.keylist.remove(event.key())
                self.processmultikeys(self.keylist)
    
    def processmultikeys(self, keyspressed):
        #TODO send eth control
        
        print(self.keylist)
        
        Key_G = QtCore.Qt.Key_G in self.keylist
        Key_J = QtCore.Qt.Key_J in self.keylist
        Key_Y = QtCore.Qt.Key_Y in self.keylist
        Key_H = QtCore.Qt.Key_H in self.keylist
        Key_Q = QtCore.Qt.Key_Q in self.keylist
        Key_E = QtCore.Qt.Key_E in self.keylist        
        
        Key_R = QtCore.Qt.Key_R in self.keylist
        
        
        Key_W = QtCore.Qt.Key_W in self.keylist
        Key_S = QtCore.Qt.Key_S in self.keylist
        
        self.device.power = 0 + Key_W - Key_S

        Key_A = QtCore.Qt.Key_A in self.keylist
        Key_D = QtCore.Qt.Key_D in self.keylist
         
        Key_I = QtCore.Qt.Key_I in self.keylist
         
        self.device.direction = 0 + Key_A - Key_D

        Key_SPACE = QtCore.Qt.Key_Space in self.keylist

        if(self.device.isConnected()):
            self.device.movePD(1 + Key_W - Key_S, 1 + Key_A - Key_D)
        

        print("ololololololo"+str(Key_W) +"|"+ str(QtCore.Qt.Key_W))
        
        self.cam.zdirection = 0 + Key_Q - Key_E
        self.cam.hdirection = 0 + Key_G - Key_J
        self.cam.vdirection = 0 + Key_Y - Key_H
        
        if(Key_I):
            self.cam.moveReset();
        else:
            
            if(Key_R):
                self.cam.home();
            else:
                self.cam.move();
            
            
        
        
        
    def closeEvent(self, event):
        # do stuff
        self.updater.stop()
        self.updater.wait()
        self.device.destroy()
        print("Goodby, World!")
        sys.exit()
    
    def exitApp(self):
        self.updater.stop()
        self.updater.wait()
        self.device.destroy()
        print("Goodby, World!")
        sys.exit() 
    def saveLog(self):
        self.logger.save()

    def updateState(self):
        BatteryState = self.device.getBatteryState()
        self.leftBatteryState.setValue(int(BatteryState[0]))
        self.rightBatteryState.setValue(int(BatteryState[1]))
        self.obstacleLeftTop.setValue(10)
        self.obstacleRightTop.setValue(10)
        self.obstacleTop.setValue(10)
        self.obstacleBottom.setValue(10)            
        
        #if BatteryState[0] <= 7000:
        #    self.logger.append("Low left battery level")
        #if BatteryState[1] <= 7000:
        #    self.logger.append("Low right battery level")
        
    def addMarkerWithLabel(self):
        doc = self.mapView.page().mainFrame().documentElement()
        #user = doc.findFirst("div[id=map_canvas]")
        #user.evaluateJavaScript("panTo(new google.maps.LatLng(51.98646, 81.90669));")
        
    def resetTelemetry(self):
        self.obstacleLeftTop.setValue(0)
        self.obstacleRightTop.setValue(0)
        self.obstacleTop.setValue(0)
        self.obstacleBottom.setValue(0)
    
    def changeConnectionState(self):
        if(self.device.isConnected()):
            self.device.disconnect()
            self.connectStateIndicator.setPixmap(self.device.disconState)
            self.actionConnect.setText("Connect")
            self.actionConnect.setShortcut('F5')
            self.device.currentState = 0
            self.resetTelemetry()
            
        else:
            if self.device.connect() == 0:
                self.connectStateIndicator.setPixmap(self.device.conState)
                self.actionConnect.setText("Disconnect")
                self.actionConnect.setShortcut('Ctrl+D')
                self.device.currentState = 1
            #self.updateTelemetry()
        
if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    window = ControlApp(sys.argv)
    app.setActiveWindow(window)
    sys.exit(app.exec_())