#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "DbManager.h"
#include "DisplaySettings.h"
#include "DisplaySettingsDAO.h"


int ds_resetFlagWrite(DbManager* dbm)
{
    sqlite3_stmt * stm;
    sqlite3 * db = dbm->db;

    char sentence[256];
    sprintf(sentence,"UPDATE DisplaySettings SET flagWrite=0 \0");
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

int ds_getListDisplaysSettings(DbManager* dbm,DisplaySettings* pList,int listSize)
{
    sqlite3 * db = dbm->db;
    sqlite3_stmt * stm;

    int k;
    for(k=0; k<listSize; k++)
    {
        DisplaySettings* ds = (pList+k);
        ds->idDisplay=-1;
    }

    char * sentence = "SELECT idDisplay,bright,flagVirtualReal FROM DisplaySettings\0";
    if(sqlite3_prepare_v2(db,sentence,strlen(sentence),&stm,0)!=SQLITE_OK)
        return -1;

    int ret = sqlite3_step(stm);

    int i=0;
    while(ret==SQLITE_ROW)
    {
        if(i>=listSize)
            break;

        DisplaySettings* ds = (pList+i);

        ds->idDisplay = sqlite3_column_int(stm,0);
        ds->bright = sqlite3_column_int(stm,1);
        ds->flagVirtualReal = sqlite3_column_int(stm,2);
        i++;

        ret = sqlite3_step(stm);
    }
    sqlite3_finalize(stm);
    return 0;
}

