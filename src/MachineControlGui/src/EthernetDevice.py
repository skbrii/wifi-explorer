'''
Created on 07.06.2013

@author: ko10ok
'''

import socket
import os
from src.Commutator import Commutator
import threading

class EthernetDevice(object):
    '''
    classdocs
    '''
 
    #default values
    BUFFER_SIZE = 1024

    currentState = 0;

    def __init__(self, logger, tcpIP= '192.168.2.100', tcpPort= 80):
        '''
        Constructor
        '''
        self.logger = logger
        self.tcpIP = tcpIP
        self.tcpPort = tcpPort
        self.currentState=0
        self.commutator = Commutator(logger);
        
        #t = threading.Thread(name="Updater",target=self.commutator.run())
        #t.start()
    
    def destroy(self):
        self.disconnect()
        self.commutator.destroy()
            
    def connect(self):
        if self.commutator.connect() == 0:
            self.currentState=1
            self.commutator.send(command=[0])
            return 0;
        return -1
   
    def disconnect(self):
        self.commutator.disconnect()
        self.currentState=0
    
    def isConnected(self):
        return self.currentState;
    
    def getTelemetry(self):
        t=1
    
    def resetDrivers(self):
        controlArray = []
        controlArray.append(0)
        print(controlArray)
        self.deviceSocket.send(bytes(controlArray))
        
    
    def sendControl(self, command, power=0, direction=0):
        controlArray = []
        controlArray.append(command)
        controlArray.append(int(power)+1)
        controlArray.append(int(direction)+1)
        print(controlArray)
        self.deviceSocket.send(bytes(controlArray))
        
        
    def getBatteryState(self):
        return self.commutator.getBatteryState()
        
    def movePD(self, power, direction):
        command = []
        command.append(1)
        command.append(power)
        command.append(direction)
        self.commutator.send(command)

    def moveWSAD(self, key_w, key_s, key_a, key_d):
        command = []
        command.append(10)
        command.append(key_w)
        command.append(key_s)
        command.append(key_a)
        command.append(key_d)
        self.commutator.send(command)
    
    def breakMove(self):
        command = []
        command.append(0)
        self.commutator.send(command)
    
    @DeprecationWarning
    def sendCommand(self, command, args=[]):
        controlArray = []
        controlArray.append(command)
        for i in range(len(args)): 
            controlArray.append(args[i])
        print(controlArray)
        
        #self.deviceSocket.send(bytes(controlArray))
