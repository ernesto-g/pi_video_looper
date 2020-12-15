#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <syslog.h>
#include <signal.h>

#include "../comm/Rs232Layer.h"
#include "../database/DbManager.h"
#include "../database/VarDAO.h"
#include "../comm/Protocol.h"
#include "HardwareValidator.h"
#include "ServiceLogic.h"
#include "../comm/SocketServerManager.h"
#include "Configuracion.h"

void captureSignals(void);
void receivePacketsFromSockets(char* pData,int len);
Port port;


int main(int argc, char** argv) {

    //Port port;
    DbManager dbm;

    int i;

	captureSignals();
	openlog ("serviceCommBBB", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	syslog (LOG_NOTICE, "Starting Comm BBB service");

    port.bdrate=9600;
    port.cport_nr=PORT_TTYO4;

    printf("Abriendo TTYO4...\n");
    int r = rs232OpenPort(&port);
    if(r!=0)
    {
        syslog(LOG_ERR,"Opening TTYO4");
        return (EXIT_FAILURE);
    }

    printf("Abriendo Base de datos...\n");
    if(dbm_openDb(&dbm,"/opt/remote_video_player/commService/dbntp.db",0)!=0)
    //if(dbm_openDb(&dbm,"dbcontroladora.db",0)!=0)
    {
        syslog(LOG_ERR,"Opening database");
        return (EXIT_FAILURE);
    }


    setDateTime(&dbm,2000,01,01,0,0,0);

	runNTP(&port,&dbm);

    ssm_init();

    char buffRx[256];
    BufferUart buffers[32];
    printf("Recibiendo...\n");
    syslog(LOG_NOTICE,"service started OK");
    int hardValidationCounter=0;
    while(1)
    {
        //usleep(1000000);
        ssm_poolClientsPackets(receivePacketsFromSockets);

        int rr = rs232ReceiveWithTimeout(&port,buffRx,sizeof(buffRx),10);
        //printf("loop!\n");
        if(rr!=0)
        {
            /*
            printf("se recibieron %d bytes\n",rr);
	    	for(i=0; i<rr;i++)
				printf("0x%02x:",buffRx[i]);
	    	printf("\n");*/
            
            int buffersLen =  protocol_SplitBuffer(buffRx,rr,buffers,32);
            //printf("Hay %d paquetes\r\n",buffersLen);

            int indexBuffers;
            for(indexBuffers=0; indexBuffers<buffersLen; indexBuffers++)
            {
                //printf("puntero:0x%x len:%d\r\n",buffers[indexBuffers].pData,buffers[indexBuffers].len);
	    	    Packet packet;
	    	    //if(protocol_analizePacket(buffRx,rr,&packet)==0)
	    	    if(protocol_analizePacket(buffers[indexBuffers].pData,buffers[indexBuffers].len,&packet)==0)
	    	    {
                    //printf("OK\r\n");
				    serviceLogic(&port,&dbm,&packet);
	    	    }
            }
        }

        hardValidationCounter++;
        if(hardValidationCounter>=10) // cada 1 segundo
        {
            hardValidationCounter=0;
		    serviceDateTimeManager(&port,&dbm);
		    serviceHardwareValidator(&port,&dbm);
        }
		usleep(100000); // 100ms
    }

	syslog (LOG_NOTICE, "Closing Comm BBB service");
    return (EXIT_SUCCESS);
}

void receivePacketsFromSockets(char* pData,int len)
{
    //printf("envio por 232 a la cape, packet de cliente tcp, len:%d\r\n",len);
    rs232SendPacket(&port,pData,len);
}

void salirPorSignal(int sigNumber)
{
    syslog(LOG_ERR,"Fatal Error, signal:%d",sigNumber);
	syslog (LOG_NOTICE, "Closing Comm BBB service");
    exit(EXIT_FAILURE);
}
void captureSignals(void)
{
    signal(SIGABRT,salirPorSignal);
    signal(SIGILL,salirPorSignal);
    signal(SIGFPE,salirPorSignal);
    signal(SIGSEGV,salirPorSignal);
    signal(SIGTERM,salirPorSignal);
}

