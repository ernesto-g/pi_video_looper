from ctypes import *

pl = CDLL("./libprotocol_layer.so")

class _Protocol (object):

    CMD_SET_SCREEN_DATA = 0x33

    def __init__(self):
        pass


    def pack(self,cmd,data):
        crc = 0

        packet = bytearray()
        packet.append(0x02)
        packet.append(0x10)
        self.__appendCodifByte(packet,cmd)
        crc = self.__calculateCrcByte(cmd,crc)

        for b in data:        
            self.__appendCodifByte(packet,b)
            crc = self.__calculateCrcByte(b,crc)

        self.__appendCodifByte(packet,(crc>>8) & 0x00FF)
        self.__appendCodifByte(packet,(crc) & 0x00FF)

        packet.append(0x10)
        packet.append(0x03)

        return packet

    def unpack(self,data):
        rawData = bytearray()
        crc = 0x0000
        index = 2
        while index < len(data)-2 :
            
            if data[index] == 0x10:
                index+=1
                rawData.append(data[index]&0x7F)
                crc = self.__calculateCrcByte(data[index]&0x7F,crc)
            else:
                rawData.append(data[index])
                crc = self.__calculateCrcByte(data[index],crc)

            index+=1

        #print("PR - crc:"+str(crc))
        if crc == 0x0000:    
            return rawData[0:len(rawData)-2]

        return None


    def __calculateCrcByte(self,b,prevCrc):
        #print("PR - prev_crc:"+str(prevCrc)+" calculo crc de:"+str(int(b)))
        newCrc = pl.pl_calculateCrc(b,prevCrc)
        #print("PR - nuevo crc:"+str(newCrc))
        return newCrc&0x0000FFFF

    def __appendCodifByte(self,packet,b):
        if b==0x02:
            packet.append(0x10)
            packet.append(0x82)
        elif b==0x03:
            packet.append(0x10)
            packet.append(0x83)
        elif b==0x10:
            packet.append(0x10)
            packet.append(0x90)
        else:
            packet.append(b)

        return

    def sarasa(self):
        pass
