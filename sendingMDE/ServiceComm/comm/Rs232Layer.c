/*
 * Rs485Layer.c
 *
 *  Created on: 22/01/2013
 *      Author: Ernesto
 */

#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "rs232.h"
#include "Rs232Layer.h"


int rs232OpenPort(Port * pPort)
{
        return OpenComport(pPort->cport_nr, pPort->bdrate);
}

int rs232SendPacket(Port * pPort,unsigned char * buff,int size)
{
	int r;
	r =  SendBuf(pPort->cport_nr, buff,size);
	return r;
}

int rs232Receive(Port * pPort,unsigned char * buff,int size)
{
	return PollComport(pPort->cport_nr, buff, size);
}

/*
int rs232ReceiveWithTimeout(Port * pPort,unsigned char * bufferRx,int size,int timeout)
{
	int timeout2 = timeout/10;
	int index=0;
    while(timeout2>0)
    {
	    //printf("poll\n");
	    int r = rs232Receive(pPort,bufferRx+index,1);
	   	//printf("r:%d\n",r);
	    if(r==1)
	    {
			//printf("llego bytee\n");
			index++;
			timeout2 = timeout / 10;
			if(index>=size)
		    	break;
	    }
	    else
		{
            usleep(10000); // 10ms
            timeout2--;
	    }
    }
	//printf("FIN\n");
    return index;
}
*/


int rs232ReceiveWithTimeout(Port * pPort,unsigned char * bufferRx,int size,int timeout)
{
    int timeout2 = timeout / 5;
    int index=0;
    int sizeLeft = size;
    while(timeout2>0)
    {
        int r = rs232Receive(pPort,bufferRx+index,sizeLeft); // fix size del buffer estaba fijo en 64 tiraba seg fault
        if(r>0)
        {
            index+=r;
            //printf("llegaron %d bytes, espacio restante:%d\n",r,sizeLeft);
            timeout2 = timeout / 5;
            sizeLeft=sizeLeft-r;
            if(index>=size)
                break;
        }
        else
        {
            usleep(5000); // 1ms
            timeout2--;
        }
    }
    //printf("FIN\n");
    return index;
}

void rs232ClosePort(Port * pPort)
{
	CloseComport(pPort->cport_nr);
}

