#include "../comm/Rs232Layer.h"
#include "../database/DbManager.h"
#include "../database/VarDAO.h"
#include "../database/SysDAO.h"
#include "../comm/Protocol.h"
#include "../comm/SocketServerManager.h"
#include "Configuracion.h"
#include "ServiceLogic.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define ID_VARSYS_TEMP	0
#define ID_VARSYS_HUM	1
#define ID_VARSYS_LIGHT	2
#define ID_VARSYS_IN0	3
#define ID_VARSYS_IN1	4
#define ID_VARSYS_IN2	5
#define ID_VARSYS_RFSW  6


static int flagSetDateTime=1;

// Private functions
//void setDateTime(DbManager * dbm,int yr,int mon,int day,int hr,int min,int sec);
void setSysVars(DbManager * dbm,float temp,float hum,int light,int in0,int in1,int in2,unsigned char rfSwState);
int sendDateTimeToPic(Port* port,int yr,int mon,int day,int hr,int min,int sec);
int setUserVar(int idDisplay,int idProgram,int idVar,char* value);
int checkInternetConnection(void);
void* ntpThread(void* args);
int execManualNTP(char* ntpUrl);
//_________________

static int ntpOnOff;
static char ntpUrl[256];
static char ntpTime[256];
static pthread_t threadNtp;
static pthread_mutex_t mutexNtp;
static int flagNtpInitialized=0;

void runNTP(Port* port,DbManager *dbm)
{
    sys_readNtpConfig(dbm, &ntpOnOff, ntpUrl, ntpTime);// leo URL de DB
    //sprintf(ntpUrl,"time.nist.gov"); // default server

    execManualNTP(ntpUrl); // EJECUTO NTP
    //sleep(30);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int year =  tm.tm_year + 1900;
    if(year>=2020)
	{
        //ntp puso en hora el sistema, 
		flagSetDateTime=0; // bajo el flag para no tomar la hora del reloj del PIC
		printf("NTP OK Setting PIC clock...\n");
        // Envio hora al PIC para que quede en HORA
		sendDateTimeToPic(port,year,tm.tm_mon + 1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
        flagNtpInitialized=1;
	}
    else
    {
        // No habia internet o el server NTP no contesto
        flagSetDateTime=1; // TOMO LA HORA DESDE EL PIC HASTA QUE EL NTP ME CONTESTE
        // Lanzo thread para consultar al NTP hasta que me conteste
        pthread_mutex_init(&mutexNtp, NULL);
        pthread_create(&threadNtp, NULL, ntpThread, port);
    }

/*
    ntpOnOff=0;
    sys_readNtpConfig(dbm, &ntpOnOff, ntpUrl, ntpTime);
    if(ntpOnOff==0)
    {
        if(checkInternetConnection()==0)
            return; // No hay internet

        sprintf(ntpUrl,"time.nist.gov"); // default server
    }
    else
    {
        // habilito thread para consultar servidor ntp 1 vez por dia
        printf("Custom NTP config\n");
        pthread_mutex_init(&mutexNtp, NULL);
        pthread_create(&threadNtp, NULL, ntpThread, dbm);
    }

    pthread_mutex_lock(&mutexNtp);

    execManualNTP(ntpUrl);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int year =  tm.tm_year + 1900;
    if(year>=2020)
	{
		flagSetDateTime=0; // ntp puso en hora el sistema, bajo el flag para no tomar la hora del reloj del PIC
		printf("NTP OK Setting PIC clock...\n");
		sendDateTimeToPic(port,year,tm.tm_mon + 1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);

        // update last time ntp update
        sys_updateNtpLastUpdate(dbm);
        flagNtpInitialized=1;
	}

    if(ntpOnOff==1) // si el ntp esta habilitado, nunca cargo la hora desde el pic
        flagSetDateTime=0;

    pthread_mutex_unlock(&mutexNtp);
    */
}

int checkInternetConnection(void)
{
    char sentence[64];
    char result[64];
    FILE *f;
    sprintf(sentence,"/opt/remote_video_player/commService/internetCheck.sh");
    printf("Exec:%s\n",sentence);
    f = popen(sentence, "r");
    fscanf(f,"%s",result);
    pclose(f);
    printf("Internet status:%s\r\n",result);
    if(result[0]=='0')
        return 0;

    return 1;
}

int serviceLogic(Port *port,DbManager *dbm, Packet *packet)
{
    switch(packet->cmd)
    {
		case CMD_SENSORS_DATA:
		{
			//printf("llego comando sensonrs data, len:%d\r\n ",packet->len);
			if(packet->len!=23)
				return -1;

			float temp = *((float*)(&packet->pData[0]));
			float hum = *((float*)(&packet->pData[4]));
			int light = (int)(*((unsigned short*)(&packet->pData[8])));
			int hr = packet->pData[10];
			int min = packet->pData[11];
			int sec = packet->pData[12];
			int day = packet->pData[13];
			int mon = packet->pData[14];
			int yr = packet->pData[15] + 2000;
			int in0 = packet->pData[16];
			int in1 = packet->pData[17];
			int in2 = packet->pData[18];
			unsigned char rfSwState = packet->pData[19];
			printf("%02d/%02d/%04d %02d:%02d:%02d - temp:%03.2f hum:%03.2f light:%03d - in0:%1d in1:%1d in2:%d - rfSwState:0x%x\n",day,mon,yr,hr,min,sec,temp,hum,light,in0,in1,in2,rfSwState);

            printf("inicio setSysVars\r\n");
			setSysVars(dbm,temp,hum,light,in0,in1,in2,rfSwState);
            printf("fin setSysVars\r\n");

			// DateTime. Sin pila no cargo la hora que viene del pic
            #ifndef SIN_PILA
			if(flagSetDateTime==1)
			{
                printf("inicio setDatetime\r\n");
				setDateTime(dbm,yr,mon,day,hr,min,sec);
                printf("fin setDatetime\r\n");
				flagSetDateTime=0;
			}
            #endif
			//__________

            printf("fin break switch\r\n");
	    	break;
		}
        case CMD_SET_USER_VAR:
        {
            int idDisplay = packet->pData[0];
            int idProgram = packet->pData[1];
            int idVar = packet->pData[2];
            char * value =  &(packet->pData[3]);

            printf("Set variable[%d]:%s\r\n",idVar,value);

            if(setUserVar(idDisplay,idProgram,idVar,value)!=0)
                printf("Error updatting user var\n");

            break;
        }
        case CMD_HARD_VALIDATE:
        {
            break;
        }
        default:
        {
            // Envio trama por UDP a todos los clientes conectados
            packet->len=packet->len-3; // no cuento el comando ni los 2 bytes de CRC
            printf("CMD:0x%02x - len:%d Reenvio por TCP\r\n",packet->cmd,packet->len);
            char txBuffer[1024];
            if(txBuffer!=NULL)
            {
                int len = protocol_packData(packet,txBuffer);
                ssm_sendPacketToAllClients(txBuffer,len);
            }
        }
    }
    return 0;
}

void serviceDateTimeManager(Port* port,DbManager* dbm)
{
	int flagUpdate;
	char datetime[16];
	if(varDAO_getUpdateDateTime(dbm,&flagUpdate,datetime)==0)
	{
		if(flagUpdate==1)
		{
			datetime[14]=0x00;
			printf("Actualizar hora desde DB\n");
			printf("DateTime:%s\n",datetime);
			char str[5];
			str[0]=datetime[0];str[1]=datetime[1];str[2]=datetime[2];str[3]=datetime[3];str[4]=0;
			int yr = atoi(str);
			str[0]=datetime[4];str[1]=datetime[5];str[2]=0;
			int mon = atoi(str);
            str[0]=datetime[6];str[1]=datetime[7];str[2]=0;
            int day = atoi(str);
            str[0]=datetime[8];str[1]=datetime[9];str[2]=0;
            int hr = atoi(str);
            str[0]=datetime[10];str[1]=datetime[11];str[2]=0;
            int min = atoi(str);
            str[0]=datetime[12];str[1]=datetime[13];str[2]=0;
            int sec = atoi(str);
			//printf("%02d/%02d/%04d %02d:%02d:%02d\n",day,mon,yr,hr,min,sec);
			/*
			yr=yr-2000;
			unsigned char txBuff[8];
			txBuff[0] = (unsigned char)yr;
			txBuff[1] = (unsigned char)mon;
			txBuff[2] = (unsigned char)day;
			txBuff[3] = (unsigned char)hr;
			txBuff[4] = (unsigned char)min;
			txBuff[5] = (unsigned char)sec;
			protocol_sendPacket(port,CMD_SET_DATETIME,txBuff,6);
			char buffRx[32];
			int rr = rs232ReceiveWithTimeout(port,buffRx,sizeof(buffRx),200);
        	if(rr!=0)
			{
				printf("Llego ACK\n");
				varDAO_resetUpdateDateTime(dbm);
				flagSetDateTime=1; // para que se vuelva a cargar la hora cuando viene la trama
			}
			else
				printf("No llego respuesta\n");
			*/
			if(sendDateTimeToPic(port,yr,mon,day,hr,min,sec)==1)
			{
				varDAO_resetUpdateDateTime(dbm);
				flagSetDateTime=1; // para que se vuelva a cargar la hora cuando viene la trama
			}
		}
	}
}


int sendDateTimeToPic(Port* port,int yr,int mon,int day,int hr,int min,int sec)
{
	yr=yr-2000;
    unsigned char txBuff[8];
    txBuff[0] = (unsigned char)yr;
    txBuff[1] = (unsigned char)mon;
    txBuff[2] = (unsigned char)day;
    txBuff[3] = (unsigned char)hr;
    txBuff[4] = (unsigned char)min;
    txBuff[5] = (unsigned char)sec;
    protocol_sendPacket(port,CMD_SET_DATETIME,txBuff,6);
    char buffRx[32];
    int rr = rs232ReceiveWithTimeout(port,buffRx,sizeof(buffRx),200);
    if(rr!=0)
    {
    	printf("Llego ACK\n");
		return 1;
    }
    else
       printf("No llego respuesta\n");

	return 0;
}

void setDateTime(DbManager * dbm,int yr,int mon,int day,int hr,int min,int sec)
{
	char sentence[64];
    FILE *f;
    sprintf(sentence,"date +%%Y%%m%%d -s \"%04d%02d%02d\"",yr,mon,day);
    printf("Ejecuto:%s\n",sentence);
    f = popen(sentence, "r");
    pclose(f);

    sprintf(sentence,"date +%%T -s \"%02d:%02d:%02d\"",hr,min,sec);
    printf("Ejecuto:%s\n",sentence);
    f = popen(sentence, "r");
    pclose(f);

	// Le digo al reproductor que se resetee porque al cambiar la fecha queda colgado webkit
	sys_setPendingReboot(dbm);

}

void setSysVars(DbManager * dbm,float temp,float hum,int light,int in0,int in1,int in2,unsigned char rfSwState)
{
	Var var;
    char str[64];
    char degree[4];
    degree[0]=0xC2; degree[1]=0xB0; degree[2]='C'; degree[3]=0x00;

    //printf("init update temp\n");
    var.value=str;
    var.id=ID_VARSYS_TEMP;
    sprintf(str,"%03.1f%s",temp,degree);
    if(varDAO_updateVar(dbm,&var)!=0)
    	printf("Error updateando temperatura\n");

    //printf("init update hum\n");
    usleep(10000);
    var.id=ID_VARSYS_HUM;
    sprintf(str,"%03.2f%%",hum);
    if(varDAO_updateVar(dbm,&var)!=0)
    	printf("Error updateando humedad\n");

    //printf("init update ligh\n");
    usleep(10000);
    var.id=ID_VARSYS_LIGHT;
    sprintf(str,"%03d%%",light);
    if(varDAO_updateVar(dbm,&var)!=0)
    	printf("Error updateando light\n");

    //printf("init update in0\n");
    usleep(10000);
    var.id=ID_VARSYS_IN0;
    sprintf(str,"%01d",in0);
    if(varDAO_updateVar(dbm,&var)!=0)
    	printf("Error updateando in0\n");


    //printf("init update in1\n");
     usleep(10000);
     var.id=ID_VARSYS_IN1;
     sprintf(str,"%01d",in1);
     if(varDAO_updateVar(dbm,&var)!=0)
     	printf("Error updateando in1\n");

     usleep(10000);
     var.id=ID_VARSYS_IN2;
     sprintf(str,"%01d",in2);
     if(varDAO_updateVar(dbm,&var)!=0)
     	printf("Error updateando in2\n");
    
     usleep(10000);
     var.id=ID_VARSYS_RFSW;
     sprintf(str,"%01d",rfSwState);
     if(varDAO_updateVar(dbm,&var)!=0)
     	printf("Error updateando switches RF\n");
}

int setUserVar(int idDisplay,int idProgram,int idVar,char* value)
{
    DbManager dbm;
    char path [256];
    sprintf(path,"/var/www/Controladora/Displays/%d/ws/mediaContentManager/resourcesDatabase/dbcartel_%d.db",idDisplay,idProgram);
    if(dbm_openDb(&dbm,path,0)!=0)
        return 1;

    Var var;
    var.value=value;
    var.id=idVar;
    if(varDAO_updateUserVar(&dbm,&var)!=0)
    {
        return 1;
    }

    return 0;
}


void* ntpThread(void* args)
{
    //DbManager* dbm = (DbManager*)args;
    Port* port = (Port*)args;

    printf("NTP TH> Init.\n");
    while(1)
    {
        sleep(60);
        printf("NTP TH> tick\n");
        
        pthread_mutex_lock(&mutexNtp);
        if(checkInternetConnection()==1)
        {
            printf("NTP TH> Internet OK. Executing NTP...\n");
            // Volvio internet, pruebo consultar el server NTP
            // para ajustar la hora desde internet
            execManualNTP(ntpUrl);
            // chequeo si funciono el ntp y levanto el flag
            // flagNtpInitialized=1
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            int year =  tm.tm_year + 1900;
            if(year>=2020)
            {
                flagNtpInitialized=1;
                flagSetDateTime=0; // bajo el flag para no tomar la hora del reloj del PIC
                // Envio hora al PIC para que quede en HORA
                printf("NTP TH> Sending Date to PIC RTC...\n");

                sendDateTimeToPic(port,year,tm.tm_mon + 1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);

                printf("NTP TH> Date OK. Finishing thread...\n");
                pthread_mutex_unlock(&mutexNtp);
                return NULL; // FIN THREAD
            }
            else
            {
                printf("NTP TH> Date ERROR. Retrying...\n");
            }
        }
        pthread_mutex_unlock(&mutexNtp);
        /*
        if(ntpOnOff==1)
        {
            pthread_mutex_lock(&mutexNtp);

            printf("NTP enabled\n");
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            char aux[6];
            sprintf(aux,"%02d:%02d",tm.tm_hour,tm.tm_min);
            //printf("comparo '%s' con '%s'\n",aux,ntpTime);
            if(strncmp(ntpTime,aux,6)==0 || flagNtpInitialized==0)
            {
                printf("NTP update time\n");
                execManualNTP(ntpUrl);
                // chequeo si funciono el ntp y levanto el flag
                // flagNtpInitialized=1
                time_t t = time(NULL);
                struct tm tm = *localtime(&t);
                int year =  tm.tm_year + 1900;
                if(year>=2020)
                {
                    flagNtpInitialized=1;
                }

                // Le digo al reproductor que se resetee porque al cambiar la fecha queda colgado webkit
                sys_setPendingReboot(dbm);
                sleep(1);
                // update last time ntp update
                sys_updateNtpLastUpdate(dbm);
                sleep(65);
            }
            pthread_mutex_unlock(&mutexNtp);
        }*/
    }
}

int execManualNTP(char* ntpUrl)
{
    char sentence[1024];
    FILE *f;
    sprintf(sentence,"/opt/remote_video_player/commService/ntpdate_manual -s %s",ntpUrl);
    printf("Exec:%s\n",sentence);
    f = popen(sentence, "r");
    pclose(f);
}

