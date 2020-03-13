#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "../comm/Rs232Layer.h"
#include "../comm/Protocol.h"


void captureSignals(void);
static int getBrightFromFile(void);

int main(int argc, char** argv) {

    Port port;
    //DbManager dbm;

    int i;

	captureSignals();
	openlog ("serviceBright", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	//syslog (LOG_NOTICE, "Starting Brigth service");

    // Serial port
    port.bdrate=115200;
    port.cport_nr=PORT_TTYUSB1;
    printf("Abriendo TTYUSB0...\n");
    int r = rs232OpenPort(&port);
    if(r!=0)
        return (EXIT_FAILURE);
    //____________


    while(1)
    {
	int br = getBrightFromFile();

	printf("envio brillo:%d\n",br);
	sendBrightConfiguration(&port,br,br,br,CONFIG_REAL,HARDWARE_SCREEN_0);

	usleep(5000000);
    }

    rs232ClosePort(&port);

    syslog (LOG_NOTICE, "Closing Bright service");
    return (EXIT_SUCCESS);
}


void salirPorSignal(int sigNumber)
{
    syslog(LOG_ERR,"Fatal Error, signal:%d",sigNumber);
	syslog (LOG_NOTICE, "Closing Bright Service");
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



static int getBrightFromFile(void)
{
	int br=128;
	FILE* fp = fopen("/home/pi/bright.txt","r");
	char buffer[8];

	if(fp!=NULL)
	{
		//printf("abri archivo\n");
		bzero(buffer,8);
		fread(buffer,8,1,fp);

		//printf("lei:%s\n",buffer);
		buffer[7]=0;
		// convierto a numero
		if(sscanf(buffer,"%d",&br)==1)
		{
			printf("Lei brillo :%d\n",br);
		}

		fclose(fp);
	}
	return br;
}

