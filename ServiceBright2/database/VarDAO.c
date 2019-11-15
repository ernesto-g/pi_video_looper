#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "VarDAO.h"
#include "DbManager.h"


int varDAO_updateVar(DbManager * dbm,Var * var)
{
	sqlite3_stmt * stm;
	sqlite3 * db = dbm->db;

	char sentence[256];
	sprintf(sentence,"UPDATE SysVars SET value='%s*' WHERE id=%d\0",var->value,var->id);
	if(sqlite3_prepare_v2(db,sentence,strlen(sentence),&stm,0)!=SQLITE_OK)
	{
		return -1;
	}

	if(sqlite3_step(stm)!=SQLITE_DONE) // ROW
	{
		sqlite3_finalize(stm);
		return -1;
	}

	sqlite3_finalize(stm);
	return 0;
}

int varDAO_updateUserVar(DbManager * dbm,Var * var)
{
    sqlite3_stmt * stm;
    sqlite3 * db = dbm->db;

    char sentence[256];
    sprintf(sentence,"UPDATE UserVars SET value='%s*' WHERE id=%d\0",var->value,var->id);
    if(sqlite3_prepare_v2(db,sentence,strlen(sentence),&stm,0)!=SQLITE_OK)
    {
        return -1;
    }

    if(sqlite3_step(stm)!=SQLITE_DONE) // ROW
    {
        sqlite3_finalize(stm);
        return -1;
    }

    sqlite3_finalize(stm);
    return 0;
}


int varDAO_getUpdateDateTime(DbManager* dbm, int *flagUpdate, char* datetime)
{
	sqlite3 * db = dbm->db;
	sqlite3_stmt * stm;

	char * sentence = "SELECT flagUpdate,datetime FROM DateTime\0";
	if(sqlite3_prepare_v2(db,sentence,strlen(sentence),&stm,0)!=SQLITE_OK)
		return -1;

	int ret = sqlite3_step(stm);
	if(ret==SQLITE_ROW)
	{
		*flagUpdate = sqlite3_column_int(stm,0);
		char * datetimeAux = (char*)sqlite3_column_text(stm,1);
		memcpy(datetime,datetimeAux,strlen(datetimeAux));
		sqlite3_finalize(stm);
		return 0;
	}

	sqlite3_finalize(stm);
	return -1;
}

int varDAO_resetUpdateDateTime(DbManager* dbm)
{
    sqlite3_stmt * stm;
    sqlite3 * db = dbm->db;

    char sentence[256];
    sprintf(sentence,"UPDATE DateTime SET flagUpdate=0 \0");
    if(sqlite3_prepare_v2(db,sentence,strlen(sentence),&stm,0)!=SQLITE_OK)
    {
        return -1;
    }

    if(sqlite3_step(stm)!=SQLITE_DONE) // ROW
    {
        sqlite3_finalize(stm);
        return -1;
    }

    sqlite3_finalize(stm);
    return 0;
}


