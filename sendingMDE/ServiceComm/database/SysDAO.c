#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "DbManager.h"
#include "SysDAO.h"

int sys_setPendingReboot(DbManager* dbm)
{
    sqlite3_stmt * stm;
    sqlite3 * db = dbm->db;

    char sentence[256];
    sprintf(sentence,"UPDATE Reboot SET flagPendingReboot=1 \0");
    //printf(sentence);
    if(sqlite3_prepare_v2(db,sentence,strlen(sentence),&stm,0)!=SQLITE_OK)
        return -1;

    if(sqlite3_step(stm)!=SQLITE_DONE) // ROW
    {
        sqlite3_finalize(stm);
        return -1;
    }

    sqlite3_finalize(stm);
    return 0;
}


int sys_readNtpConfig(DbManager* dbm, int* pNtpOnOff, char* pNtpServer, char* pNtpTime)
{
    sqlite3 * db = dbm->db;
    sqlite3_stmt * stm;

    char * sentence = "SELECT url,onOff,refreshTime FROM NtpSettings\0";
    if(sqlite3_prepare_v2(db,sentence,strlen(sentence),&stm,0)!=SQLITE_OK)
        return -1;

    int ret = sqlite3_step(stm);
    if(ret==SQLITE_ROW)
    {
        char* strAux = (char*)sqlite3_column_text(stm,0);
        memcpy(pNtpServer,strAux,strlen(strAux));

        *pNtpOnOff = sqlite3_column_int(stm,1);

        strAux = (char*)sqlite3_column_text(stm,2);
        memcpy(pNtpTime,strAux,strlen(strAux));

        sqlite3_finalize(stm);
        return 0;
    }

    sqlite3_finalize(stm);
    return -1;
}

int sys_updateNtpLastUpdate(DbManager* dbm)
{
    sqlite3_stmt * stm;
    sqlite3 * db = dbm->db;

    time_t t = time(NULL);
    char strNow[32];
    strftime(strNow, 20, "%Y-%m-%d %H:%M:%S", localtime(&t));

    char sentence[256];
    sprintf(sentence,"UPDATE NtpSettings SET lastUpdate='%s' \0",strNow);
    if(sqlite3_prepare_v2(db,sentence,strlen(sentence),&stm,0)!=SQLITE_OK)
        return -1;

    if(sqlite3_step(stm)!=SQLITE_DONE) // ROW
    {
        sqlite3_finalize(stm);
        return -1;
    }

    sqlite3_finalize(stm);
    return 0;
}

