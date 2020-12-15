#include <stdio.h>
#include <syslog.h>
#include <unistd.h>

#include "DbManager.h"
#include "SettingsDAO.h"


void setOut(int val)
{
    FILE *f;

    f = fopen("/sys/class/gpio/gpio26/value", "a");
    if(val==1)
        fprintf(f, "1");
    else
        fprintf(f, "0");

    fclose(f);
}

int getIn(void)
{
    FILE *f;
    char buffer[1];
    f = fopen("/sys/class/gpio/gpio25/value", "r");
    fread(&buffer, 1, 1, f);
    fclose(f);

    if(buffer[0]==0x30)
        return 0;

    return 1;
}

void resetIp(void);


int main(void)
{
    int flagToogle=1;

    openlog ("serviceWatchdog", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    syslog (LOG_NOTICE, "Starting Watchdog service");

    while(1)
    {
        if(flagToogle==1)
        {
            flagToogle=0;
            setOut(0);
        }
        else
        {
            flagToogle=1;
            setOut(1);
        }

        usleep(1000000); // 1 seg

        if(getIn()==0)
        {
            setOut(1);
            int t=20;
            while( (getIn()==0) && t>0 )
            {
                usleep(100000); // 100ms
                t--;
            }
            setOut(0);
            usleep(1000000); // 1 seg
            setOut(1);

            if(t<=0)
            {
                resetIp();
            }
        }
    }
}


void resetIp(void)
{
    syslog (LOG_NOTICE, "Restart default IP");

    DbManager db;
    if(dbm_openDb(&db,"/opt/remote_video_player/commService/dbntp.db",0)==0)
    {
        settingsDAO_resetUserPassWeb(&db);
        dbm_closeDb(&db);
    }


    char sentence[1024];
    FILE *f;
    sprintf(sentence,"cp /opt/remote_video_player/wdtService/dhcpcd.conf /etc/dhcpcd.conf");
    printf("Exec:%s\n",sentence);
    f = popen(sentence, "r");
    pclose(f);


    sprintf(sentence,"cp /opt/remote_video_player/wdtService/hostapd.conf /etc/hostapd/hostapd.conf");
    printf("Exec:%s\n",sentence);
    f = popen(sentence, "r");
    pclose(f);


    sprintf(sentence,"reboot");
    printf("Exec:%s\n",sentence);
    f = popen(sentence, "r");
    pclose(f);

}



