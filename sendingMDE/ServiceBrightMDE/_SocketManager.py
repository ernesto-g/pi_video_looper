import socket
from threading import Thread,Lock
import time


TCP_IP = '127.0.0.1'
BUFFER_SIZE = 4096

class _SocketManager(object):

    def __init__(self,port,packetCallback,conectionErrorCallback):

        self.connected = False
        self.port = port
        self.packetCallback = packetCallback
        self.conectionErrorCallback = conectionErrorCallback
        self.__sendMutex = Lock()
        self.__reconnect()
        


    def __receiveInThread(a,packetCallback,s,this):
        #packetCallback = args[0]
        #s = args[1]
        while True:
            #print("SM - espero recepcion por socket en thread")
            #syslog.syslog(syslog.LOG_INFO,'SM - espero recepcion por socket en thread')
            try:
                data = s.recv(BUFFER_SIZE)
            except:
                data=""

            if data == "":
                #print("SM - se cerro la conexion con el servidor")
                #syslog.syslog(syslog.LOG_INFO,'SM - se cerro la conexion con el servidor')
                s.close()
                this.connected = False
                if this.conectionErrorCallback!=None:
                    this.conectionErrorCallback()
                break
            #print("SM - llego paquete, llamo a callback")
            if packetCallback!=None:
                packetCallback(bytearray(data))

    def sendData(self,data):
        self.__sendMutex.acquire()
        try:
            if self.connected==True:
                #print("envio:"+str(data))
                self.s.send(data)
                self.__sendMutex.release()
                return True
        except:
            pass
            #print("SM - Error enviando paquete")
        self.__sendMutex.release()    
        return False

    def __reconnect(self):
        while True:
            if self.__reconnect2():
                return True
            print("SM - esperando servidor...")
            #syslog.syslog(syslog.LOG_INFO,'SM - esperando servidor...')
            time.sleep(1)
        return False
    
    def __reconnect2(self):
        try:
            self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.s.connect((TCP_IP, self.port))
            self.thread = Thread(target = self.__receiveInThread, args = [self.packetCallback,self.s,self])
            self.thread.daemon = True
            self.thread.start()
            self.connected = True
            return True
        except:
            return False

    def isConnected(self):
        return self.connected

    def close(self):
        try:
            self.s.close()
        except:
            pass

