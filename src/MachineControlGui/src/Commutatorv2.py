'''
Created on 25.06.2013

@author: SCB
'''

import socket
from symbol import except_clause
from threading import Timer, Thread
from time import sleep

from PyQt4 import QtGui, QtCore

import threading
from sqlite3.dbapi2 import Timestamp

class Commutatorv2(object):
    '''
    classdocs
    '''

    #Queue of commands to send
    commands = []

    
    
    connectionState = 0
    
    #Status registers
    GPSState = {}
    batteryState = [6400,6400]
    sensorsState =[]
    
    #Multi Threading Variables
    runningFlag = False
    updater = None;
    
    tcpIP="192.168.2.105"
    tcpPort = 80
        
    def __init__(self, tcpIP= '192.168.2.105', tcpPort = 80, listener = True):
        ''' Constructor makes a tread and runs the run method'''
        self.tcpIP = tcpIP
        self.tcpPort = tcpPort
        self.connectionState=0
        self.runningFlag = True
        
        if (listener == True):
            self.updater = threading.Thread(target=self.receive)
            self.updater.start()
    
    def getPage(self, URL = str()):
        self.camSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.camSocket.settimeout(1)
            self.camSocket.connect((self.tcpIP, self.tcpPort))
            self.camSocket.settimeout(None)
            self.camSocket.send(URL.encode())
            
        except socket.timeout:
            print("Time out error: device unavailable in this network")
        self.camSocket.close()
        
    def destroy(self):
        self.runningFlag = False
        self.updater.join();
    
    def connect(self):
        if(self.connectionState):
            self.logger.append("Already connected")
            return -1
        else:
            self.deviceSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                self.deviceSocket.settimeout(1)
                self.deviceSocket.connect((self.tcpIP, self.tcpPort))
                self.deviceSocket.settimeout(None)
                self.send([0])
            except socket.timeout:
                self.logger.append("Time out error: device unavailable in this network")
                return -1
            self.connectionState=1
            self.logger.append("Connected successfull")
            return 0
    
    def disconnect(self):
        if(self.connectionState):
            '''self.socket close'''
            self.connectionState=0;
            self.deviceSocket.close()
            self.logger.append("Disconnected")
    
    
    def getBatteryState(self):
        return self.batteryState
    
    def getTelemetryState(self):
        ''' return the array of telemetry measurements '''
        telemetry = []
        telemetry.append(self.GPSState)
        telemetry.append(self.batteryState)
        telemetry.append(self.sensorsState)
        return self.telemetry
    
    def send(self,command):
        ''' add command for transmition'''
        self.commands.append(command)
        self.transmit()
    
    
    #should be in another thread      
    def receive(self):
        ''' rx ethernet action'''
        while self.runningFlag:
            #print(self.connectionState)
            if self.connectionState:
                try:
                    #get header = data type
                    fourbyte = self.deviceSocket.recv(4)
                    print(fourbyte)
                    #get next part of message and put into register
                    if fourbyte[0] == 36:
                        self.batteryState[0] = self.deviceSocket.recv(4)
                        self.batteryState[1] = self.deviceSocket.recv(4)
                        
                        self.GPSState["longitude"] = self.deviceSocket.recv(7)
                        self.GPSState["latitude"] = self.deviceSocket.recv(7)
                        self.GPSState["altitude"] = self.deviceSocket.recv(3)
                        self.GPSState["time"] = self.deviceSocket.recv(10)
                        
                        print(self.batteryState)
                        print(self.GPSState["longitude"],end=' ')
                        print(self.GPSState["latitude"],end=' ')
                        print(self.GPSState["altitude"],end=' ')
                        print(self.GPSState["time"])
                        
                except (ConnectionAbortedError, ConnectionResetError):
                    #disconnected
                    continue               
            
        print("Receiver ended")

    def transmit(self):
        ''' tx ethernet action '''
        while len(self.commands) > 0:
            command = self.commands.pop(0)
            self.deviceSocket.send(bytes(command))
            print(command)

        