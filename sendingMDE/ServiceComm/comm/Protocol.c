#include "Rs232Layer.h"
#include "Protocol.h"

// Private Functions
unsigned short calcCrcArray(unsigned char *data, int number_of_bytes, unsigned short prev_CRC);
unsigned short calcCrcByte(unsigned char data, unsigned short prevCrc);
unsigned char *decodifPacket (unsigned char *pData, int len,int *lenOut);
//_________________


int protocol_analizePacket(unsigned char * buff, int size, Packet * packet)
{
    packet->pData = 0x00;

    // Decode
    int lenOut;
    unsigned char *p = decodifPacket(buff,size,&lenOut);
    int i;

    if(calcCrcArray(p, lenOut,0)!=0)
    {
	//printf("no dio el crc\n");
    	return -1;
    }
    packet->cmd = p[0];
    packet->pData = &(p[1]);
	packet->len = lenOut;

    return 0; // OK
}


int codifByte(unsigned char byte, unsigned char* txBuff,int index)
{
        if(byte==0x02)
        {
            txBuff[index]=0x10;
            index++;
            txBuff[index]=0x82;
            index++;
        }
        else if(byte==0x03)
        {
            txBuff[index]=0x10;
            index++;
            txBuff[index]=0x83;
            index++;
        }
        else if(byte==0x10)
        {
            txBuff[index]=0x10;
            index++;
            txBuff[index]=0x90;
            index++;
        }
        else
        {
            txBuff[index]=byte;
            index++;
        }
	return index;
}
void protocol_sendPacket(Port* port,unsigned char cmd,unsigned char* pdata,int len)
{
	unsigned char txBuff[256];
	int index=2;
	int i=0;
	unsigned short crc=0x0000;
	txBuff[0]=0x02;
	txBuff[1]=0x10;

	index = codifByte(cmd,txBuff,index);
	crc = calcCrcByte(cmd, crc);

	while(i<len)
	{
		index = codifByte(pdata[i],txBuff,index);
		crc = calcCrcByte(pdata[i], crc);
		i++;
	}

	//txBuff[index]=*(((unsigned char*)&crc)+1);
	//index++;
    index = codifByte(*(((unsigned char*)&crc)+1),txBuff,index);

	//txBuff[index]=*(((unsigned char*)&crc)+0);
	//index++;
    index = codifByte(*(((unsigned char*)&crc)+0),txBuff,index);

	txBuff[index]=0x10;
	index++;
	txBuff[index]=0x03;
	index++;
	rs232SendPacket(port,txBuff,index);
}


unsigned char *decodifPacket (unsigned char *pData, int len,int *lenOut)
{
    unsigned char *Pwrite,*Pinicio;
    unsigned int i;

    Pwrite = pData;
    Pinicio = Pwrite+2; // salteo 0x10 0x02

    i=0;
    while (Pinicio<=(pData+len))
    {
        i++;
        if((*Pinicio)==0x10 && *(Pinicio+1)==0x90)
        {
            *Pwrite=0x10;
            Pwrite++;
            Pinicio=Pinicio+2; // salteo el 0x0e
            continue;
        }
        if((*Pinicio)==0x10 && *(Pinicio+1)==0x83)
        {
            *Pwrite=0x03;
            Pwrite++;
            Pinicio=Pinicio+2; // salteo el 0x03
            continue;
        }
        if((*Pinicio)==0x10 && *(Pinicio+1)==0x82)
        {
            *Pwrite=0x02;
            Pwrite++;
            Pinicio=Pinicio+2; // salteo el 0x02
            continue;
        }

        *Pwrite=*Pinicio;
        Pwrite++;
        Pinicio=Pinicio+1; // salteo el 0x0e
    }

    //** borro los ultimos dos bytes
    *Pwrite=0;
    Pwrite++;
    *Pwrite=0;
    Pwrite--;

    *lenOut = i - 3 ;
    return Pwrite-(i);
}
//____________________________________________________________________________________________


unsigned short calcCrcArray(unsigned char *data, int number_of_bytes, unsigned short prev_CRC)
{
    while (number_of_bytes>0)
    {
	prev_CRC=calcCrcByte(*data,prev_CRC);
	data++;
	number_of_bytes--;
    }
    return prev_CRC;
}

unsigned short calcCrcByte(unsigned char data, unsigned short prevCrc)
{
    unsigned short Reg=0, Acc=prevCrc, Tmp;
    unsigned short j;

    Reg = (data<<8) & 0xFF00;

    for (j=0; j<8; j++)
    {
      Tmp = Reg ^ Acc;

      Acc = Acc << 1;

      if ((Tmp & 0x8000) != 0)
        Acc = Acc ^ 0x1021;

      Reg = Reg << 1;
    }
    return Acc;
}


int protocol_SplitBuffer(char* buffRx,int len, BufferUart* buffers,int buffersLen)
{
    int index;
    int indexBuffers=0;
    int flagStart=0;
    for(index=0; index < len ; index++)
    {
        if(buffRx[index]==0x02)
        {
            buffers[indexBuffers].pData=&buffRx[index];
            flagStart=1;
        }
        else if(buffRx[index]==0x03 && flagStart==1)
        {
            flagStart=0;
            buffers[indexBuffers].len= (&buffRx[index]) - (buffers[indexBuffers].pData) + 1;
            indexBuffers++;
            if(indexBuffers>=buffersLen)
                return indexBuffers;
        }
    }
    return indexBuffers;
}

int protocol_packData(Packet* packet,char* buffer)
{
    int i=0;
    unsigned short crc=0x0000;

    buffer[i]=0x02;
    i++;
    buffer[i]=0x10;
    i++;
    i = codifByte(packet->cmd,buffer,i);
    crc = calcCrcByte(packet->cmd, crc);

    int j = 0;
    while(j < packet->len)
    {
        i = codifByte(packet->pData[j],buffer,i);
        crc = calcCrcByte(packet->pData[j], crc);
        j++;
    }
    i = codifByte(*(((unsigned char*)&crc)+1),buffer,i);
    i = codifByte(*(((unsigned char*)&crc)+0),buffer,i);
    buffer[i]=0x10;
    i++;
    buffer[i]=0x03;
    i++;
    //printf("pack de %d bytes\r\n",i);
    return i;
}
