#!/usr/bin/python3
import time
from _Protocol import _Protocol
from _SocketManager import _SocketManager
from DisplayDAO import DisplayDAO
from ConfigServer import ConfigServer
from MyLog import MyLog

logger = MyLog.getLogger("/opt/service_bright/")
logger.info("Service bright for sending MDE")


class Main:

    SERVICE_COMM_PORT = 3000
    CONFIGURATION_SERVER_PORT = 8000

    def __init__(self):        
        self.__displayDao = DisplayDAO("/home/pi/bright.txt")
        self.__prevBright=-1
        self.__server = None

    def main(self):
        # me conecto a servicio de comunicaciones
        sm = _SocketManager(Main.SERVICE_COMM_PORT,self.__service_comm_response,None)
        prot = _Protocol()
        # levanto server para soft de configuracion
        self.__server = ConfigServer(Main.CONFIGURATION_SERVER_PORT,sm,prot)
        self.__server.start()

        # bucle envio de brillo
        logger.info("Init OK.")
        while True:
            #leo cada 2 segundos, si cambio, envio
            #aunque no haya cambiado, a los 30seg envio
            for i in range(15):
                bright,valueChanged = self.__readBright()
                if valueChanged:
                    logger.info("Bright level changed:{}".format(bright))
                    break
                time.sleep(2)
            if self.__server.is_waiting_response()==False:
                print("Sending bright:{} to FPGA".format(bright))
                data = self.__prepare_data_to_send(bright)
                data = prot.pack(_Protocol.CMD_SET_SCREEN_DATA,data)
                sm.sendData(data)
            else:
                print("No envio brillo, estoy esperando respuesta de trama de configuracion")
        #_____________________________________________

    def __prepare_data_to_send(self,bright):
            data=bytearray()
            #formato trama de brillo en fpga
            data.append(0xA0) 
            data.append(0xA1)
            data.append(0xA2)
            data.append(bright) #valor del brillo 0 a 255
            data.append(0xE0)
            data.append(0xE1)
            data.append(0xE2)
            return data

    def __readBright(self):
        ret=False
        try:
            bright = self.__displayDao.getBright()
            if bright>255:
                bright=255
            if bright<0:
                bright=0
            if bright!=self.__prevBright:
                ret=True
                self.__prevBright = bright
        except Exception as e:
            print(e)
            bright = 128

        return (bright,ret) #devuelvo el brillo y si es un nuevo valor o no

    def __service_comm_response(self,data):
        print(data)
        if self.__server!=None:
            self.__server.response_event(data)

try:
    m = Main()
    m.main()
except Exception as e:
    logger.error(e)
    logger.error("Service finished")
