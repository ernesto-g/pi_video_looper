#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <syslog.h>
#include "../comm/Rs232Layer.h"
#include "../comm/Protocol.h"
#include "../database/DbManager.h"
#include "HardwareValidator.h"

#define MAX_VALIDATION_FAILS	5
#define PERIOD_HARD_VALIDATION	30

// Private functions
int sendHardValitateToPic(Port* port,Code* code,unsigned char *hash,DbManager *dbm);
int compareHashes(unsigned char* hash,unsigned char* hash2);
void shutDown(void);
//__________________

int validationFailsCounter;
int periodMultiplier;

void serviceInitHardwareValidator(void)
{
    srand(time(NULL));
    validationFailsCounter=0;
	periodMultiplier=PERIOD_HARD_VALIDATION;
}

void serviceHardwareValidator(Port* port,DbManager *dbm)
{
	periodMultiplier--;
	if(periodMultiplier>0)
		return;
	periodMultiplier=PERIOD_HARD_VALIDATION;

	printf("Executing hardware validation...\n");

	unsigned char hash[10];
    unsigned int r0 = rand();
    unsigned int r1 = rand();
	Code code;
	code.dataLong[0]=r0;
	code.dataLong[1]=r1;
	calculateHash(hash,&code);

	int r = sendHardValitateToPic(port,&code,hash,dbm);
	if(r!=0)
	{
		validationFailsCounter++;
		if(r==2)
		{
    		printf("Invalid Hardware verification. Communication Error\n");
    		syslog (LOG_ERR,"Invalid Hardware verification. Communication Error\n");
		}
		else
		{
    		printf("Invalid Hardware verification. Number:[%d] Hash:%02x::%02x::%02x::%02x::%02x::%02x::%02x::%02x\n",validationFailsCounter,code.dataByte[0],code.dataByte[1],code.dataByte[2],code.dataByte[3],code.dataByte[4],code.dataByte[5],code.dataByte[6],code.dataByte[7]);
    		syslog (LOG_ERR,"Invalid Hardware verification. Number:[%d] Hash:%02x::%02x::%02x::%02x::%02x::%02x::%02x::%02x\n",validationFailsCounter,code.dataByte[0],code.dataByte[1],code.dataByte[2],code.dataByte[3],code.dataByte[4],code.dataByte[5],code.dataByte[6],code.dataByte[7]);
		}
		if(validationFailsCounter>=MAX_VALIDATION_FAILS)
		{
			printf("Invalid hardware. Shutting down system...\n");
			syslog (LOG_ERR,"Invalid hardware. Shutting down system...");
			shutDown();
		}
	}
	else
	{
		printf("Correct Hardware verification\n");
		validationFailsCounter=0;
	}

}


int sendHardValitateToPicInternal(Port* port,Code* code,unsigned char *hash,DbManager *dbm)
{
    protocol_sendPacket(port,CMD_HARD_VALIDATE,&(code->dataByte[0]),8);
    char buffRx[256];
    BufferUart buffers[32];
    //syslog (LOG_NOTICE, "Envio trama...");
    int rr = rs232ReceiveWithTimeout(port,buffRx,sizeof(buffRx),200);
    //debug
    /*
	int j=0;
	printf("Packet:");
	for(j=0; j<rr; j++)
	{
		printf("0x%x ",buffRx[j]);
	}
	printf("\n");
    */
    //______

    if(rr!=0)
    {
	//syslog (LOG_NOTICE, "Hay respuesta");
        int buffersLen =  protocol_SplitBuffer(buffRx,rr,buffers,32);
        int indexBuffers;
        for(indexBuffers=0; indexBuffers<buffersLen; indexBuffers++)
        {
	    //syslog (LOG_NOTICE, "Analizo packet");
            Packet packet;
            if(protocol_analizePacket(buffers[indexBuffers].pData,buffers[indexBuffers].len,&packet)==0)
            {
		//syslog (LOG_NOTICE, "Packet OK");
                serviceLogic(port,dbm,&packet);
                if(packet.cmd==CMD_HARD_VALIDATE){
			//syslog (LOG_NOTICE, "Comparo hash");
        	        return compareHashes(hash,packet.pData);
		}
            }
        }
    }

	return 2; // Comunication Error
}
int sendHardValitateToPic(Port* port,Code* code,unsigned char *hash,DbManager *dbm)
{
	int retries=2;
	while(retries>0)
	{
		int r = sendHardValitateToPicInternal(port,code,hash,dbm);
		if(r==0 || r==1)
			return r;
		usleep(10000);
		retries--;
	}
	return 2; // Communication Error
}


void calculateHash(unsigned char* dataRespuesta, Code* code)
{
    unsigned short AuxInt;

    // calculo respuesta
    AuxInt=calcCrcByte(code->dataByte[0],0x0000);
    AuxInt=calcCrcByte(code->dataByte[1],AuxInt);
    dataRespuesta[0]=*(((unsigned char*)&AuxInt)+0);
    dataRespuesta[1]=*(((unsigned char*)&AuxInt)+1);

    dataRespuesta[2]=(code->dataByte[2] ^ code->dataByte[3]) + 27;
    dataRespuesta[3]= (~code->dataByte[2] ^ code->dataByte[3]) + 27;

    AuxInt= dataRespuesta[1] - dataRespuesta[0] + dataRespuesta[2] + dataRespuesta[3];

    dataRespuesta[4]=*(((unsigned char*)&AuxInt)+0);
    dataRespuesta[5]=*(((unsigned char*)&AuxInt)+1);

    dataRespuesta[6]= code->dataByte[4] + 27;
//    dataRespuesta[6]= code->dataByte[4] + 26;

    dataRespuesta[7]= code->dataByte[5] + code->dataByte[4];

    dataRespuesta[8]= code->dataByte[6] << (code->dataByte[7]&0x07) ;

    dataRespuesta[9]= (code->dataByte[0]  + code->dataByte[7])|code->dataByte[3];
}

int compareHashes(unsigned char* hash,unsigned char* hash2)
{
	// debug
//	char strHash1[256];
//	char strHash2[256];
//	sprintf(strHash1,"Hash1:%02x::%02x::%02x::%02x::%02x::%02x::%02x::%02x::%02x::%02x",hash[0],hash[1],hash[2],hash[3],hash[4],hash[5],hash[6],hash[7],hash[8],hash[9]);
//	sprintf(strHash2,"Hash2:%02x::%02x::%02x::%02x::%02x::%02x::%02x::%02x::%02x::%02x",hash2[0],hash2[1],hash2[2],hash2[3],hash2[4],hash2[5],hash2[6],hash2[7],hash2[8],hash2[9]);
//	syslog (LOG_ERR,"Compare:%s with %s",strHash1,strHash2);
	//______
	int i;
	for(i=0;i<10;i++)
	{
		if(hash[i]!=hash2[i])
		{
			//syslog (LOG_ERR,"Compare ERROR");
			return 1;
		}
	}
	//syslog (LOG_ERR,"Compare OK");
	return 0;
}

void shutDown(void)
{
/*
    char sentence[64];
    FILE *f;
    sprintf(sentence,"halt");
    printf("Exec:%s\n",sentence);
    f = popen(sentence, "r");
    pclose(f);
*/
}
