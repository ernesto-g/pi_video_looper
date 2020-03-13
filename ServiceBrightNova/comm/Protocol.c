#include <string.h>
#include <stdio.h>

#include "Rs232Layer.h"
#include "Protocol.h"

void calculateCrcPacketStart(struct S_packetStart * packet)
{
    unsigned char * p = (unsigned char*) packet;
    p = p + 3; // Saltea header
    int i;
    packet->crc=0;
    for(i=0 ; i<22; i++)
    {
        packet->crc = packet->crc + *p;
        p++;
    }
}

void calculateCrcPacketBright(struct S_packetBright * packet)
{
    unsigned char * p = (unsigned char*) packet;
    p = p + 3; // Saltea header
    int i;
    packet->crc=0;
    for(i=0 ; i<22 ; i++)
    {
        packet->crc = packet->crc + *p;
        p++;
    }
}

void printPacket(unsigned char *p)
{
    int i;
    printf("TRAMA:");
    for(i=0 ; i<26;i++)
    {
        printf("%2x ",*p);
        p++;
    }
    printf("\r\n");
}

void incrementPacketNumberBright(struct S_packetBright * packet)
{
    packet->packetNumber++;
    if(packet->packetNumber>0x1F)
        packet->packetNumber=0x18;
}

void incrementPacketNumberStart(struct S_packetStart * packet)
{
    packet->packetNumber++;
    if(packet->packetNumber>0x57)
        packet->packetNumber=0x50;
}

void incrementPacketNumberEnd(struct S_packetStart * packet)
{
    packet->packetNumber++;
    if(packet->packetNumber>0x57)
        packet->packetNumber=0x50;
}


void sendBrightConfiguration(Port *port,int rBright,int gBright,int bBright,unsigned char realVirtual,unsigned char screenHardwareNumber)
{
    char head[] = {0x55, 0xAA, 0x00, 0x15, 0xFE, 0x00, 0x01, 0x00, 0xFF, 0xFF, 0x01, 0x00, 0x01, 0x00, 0x00, 0x02, 0x05, 0x00};

    char data[]={rBright,rBright,gBright,bBright,rBright};

    int checksum=0;
    int i;

    for(i=0; i<16;i++)
        checksum+=head[2+i];

    checksum+=data[0];
    checksum+=data[1];
    checksum+=data[2];
    checksum+=data[3];
    checksum+=data[4];
    checksum+=0x5555;

    char packet[256];

    memcpy(packet,head,sizeof(head));
    memcpy(packet+sizeof(head),data,sizeof(data));
    i = sizeof(head) + sizeof(data);

    packet[i] = checksum&0xFF;
    i++;
    packet[i] = (checksum>>8)&0xFF;
    i++;

    int j;
    printf(">>");
    for(j=0; j<i; j++)
        printf("0x%x ",packet[j]);
    printf("\n");

    rs232SendPacket(port,(unsigned char*)&packet,i+2);

}

/*
void sendBrightConfiguration(Port *port,int rBright,int gBright,int bBright,unsigned char realVirtual,unsigned char screenHardwareNumber)
{
    // Envio primera trama
    struct S_packetStart packetStart;
    packetStart.packetNumber = 0x57;
    packetStart.screenNumber = 0;
    char head[] = {0xD8,0x58,0x80,0xFF};
    char data[] = {0xAA,0x55,0xAA,0x55,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8};

	int k;
	for(k=0 ; k<3 ; k++)
	{
		incrementPacketNumberStart(&packetStart);
		memcpy(packetStart.head,head,sizeof(head));
		memcpy(packetStart.data,data,sizeof(data));
		calculateCrcPacketStart(&packetStart);
		rs232SendPacket(port,(unsigned char*)&packetStart,sizeof(struct S_packetStart));
		//printPacket((unsigned char*)&packetStart);
		//_____________________



		// Envio trama de data
		unsigned char zeros[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
		unsigned char address = 0x00;

		struct S_packetBright packetBright;
		memcpy(packetBright.head,head,sizeof(head));

		packetBright.packetNumber = 0x18;
		packetBright.screenNumber = screenHardwareNumber;
		packetBright.cmd = 0;
		packetBright.twentyTwo = 0x22;
		packetBright.address = address;

		struct S_dataBright * pDataBright = (struct S_dataBright *) &packetBright.data;

		pDataBright->realVirtual = realVirtual;
		memcpy(pDataBright->zeros0,zeros,7);
		pDataBright->rBright = rBright;
		pDataBright->gBright = gBright;
		pDataBright->bBright = bBright;
		pDataBright->ff = 0xFF;
		memcpy(pDataBright->zeros1,zeros,4);

		calculateCrcPacketBright(&packetBright);

		//printPacket((unsigned char*)&packetBright);
		rs232SendPacket(port,(unsigned char*)&packetBright,sizeof(struct S_packetBright));

		// trama 2
		memcpy(packetBright.data,zeros,16);
		packetBright.address+=16;
		switch(screenHardwareNumber)
        {
		    case HARDWARE_SCREEN_0: packetBright.data[0]=HARDWARE_SCREEN_CODE_0_H; packetBright.data[1]=HARDWARE_SCREEN_CODE_0_L;break;
		    case HARDWARE_SCREEN_1: packetBright.data[0]=HARDWARE_SCREEN_CODE_1_H; packetBright.data[1]=HARDWARE_SCREEN_CODE_1_L;break;
		    case HARDWARE_SCREEN_2: packetBright.data[0]=HARDWARE_SCREEN_CODE_2_H; packetBright.data[1]=HARDWARE_SCREEN_CODE_2_L;break;
		    case HARDWARE_SCREEN_3: packetBright.data[0]=HARDWARE_SCREEN_CODE_3_H; packetBright.data[1]=HARDWARE_SCREEN_CODE_3_L;break;
        }
		incrementPacketNumberBright(&packetBright);
		calculateCrcPacketBright(&packetBright);

		//printPacket((unsigned char*)&packetBright);
		rs232SendPacket(port,(unsigned char*)&packetBright,sizeof(struct S_packetBright));
		//____________________


		// 14 tramas restantes
		int j;
		for(j=0; j<14;j++)
		{
			memcpy(packetBright.data,zeros,16);
			packetBright.address+=16;
			incrementPacketNumberBright(&packetBright);
			calculateCrcPacketBright(&packetBright);

			//printPacket((unsigned char*)&packetBright);
			rs232SendPacket(port,(unsigned char*)&packetBright,sizeof(struct S_packetBright));
		}
		//____________________



		// Envio  trama cierre
		incrementPacketNumberStart(&packetStart);
		char dataEnd[] = {0xAA,0x55,0xAA,0x00,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8};
		memcpy(packetStart.data,dataEnd,sizeof(dataEnd));
		calculateCrcPacketStart(&packetStart);
		rs232SendPacket(port,(unsigned char*)&packetStart,sizeof(struct S_packetStart));
		//printPacket((unsigned char*)&packetStart);
		//_____________________

	}


}
*/

