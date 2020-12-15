import socket
import sys
from threading import Thread,Event
import time
from _Protocol import _Protocol



class ConfigServer(Thread):

    def __init__(self,port,socketManager,protocol):
        super().__init__()
        # Create a UDP socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        # Bind the socket to the port
        server_address = ('0.0.0.0', port)
        print('starting up on %s port %s' % server_address)
        sock.bind(server_address)
        self.__sock = sock
        self.__socketManager = socketManager
        self.__protocol = protocol
        self.__flag_wait_response=False
        self.__address_to_response = None
        self.__event = Event()

    def response_event(self,data):
        self.__flag_wait_response = False #llego respuesta del PIC, dejo de esperar respuesta para que se envie el brilo cada 30 seg
        if self.__address_to_response!=None:
            #envio OK al programa de configuracion            
            self.__sock.sendto(bytearray("OK","utf-8"), self.__address_to_response)
            self.__address_to_response = None
            self.__event.set()

    def is_waiting_response(self):
        return self.__flag_wait_response

    def run(self):
        print("Receiving from UDP client...")
        while True:
            data, address = self.__sock.recvfrom(4096)
            if len(data)>0:
                print("Received:")
                print(data)
                data = self.__protocol.pack(_Protocol.CMD_SET_SCREEN_DATA,data)
                self.__socketManager.sendData(data)
                self.__event.clear()
                self.__flag_wait_response=True #envie al  pic, espero respuesta
                self.__address_to_response = address
                
                
                #print("Entro a timer que espera respuesta...")
                self.__event.wait(2)
                #print("Salgo de timer que espera respuesta...")
                self.__flag_wait_response=False
                self.__address_to_response=None

            else:
                time.sleep(1)
            