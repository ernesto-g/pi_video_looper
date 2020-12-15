#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "DbManager.h"


int settingsDAO_resetUserPassWeb(DbManager * dbm)
{
	sqlite3_stmt * stm;
	sqlite3 * db = dbm->db;

    //printf("vardao 0-");
	char sentence[512];
	sprintf(sentence,"UPDATE SignSettings SET webLogName='admin', webLogPass='d033e22ae348aeb5660fc2140aec35850c4da997', timeoutLogin=3600\0");
	//printf("1-");
    if(sqlite3_prepare_v2(db,sentence,strlen(sentence),&stm,0)!=SQLITE_OK)
	{
		return -1;
	}
    //printf("2-");
    if(stm==NULL)
        printf("STATEMENT IS NULL\r\n");

    //printf("3-");
	if(sqlite3_step(stm)!=SQLITE_DONE) // ROW
	{
		sqlite3_finalize(stm);
		return -1;
	}
    //printf("4-");
	sqlite3_finalize(stm);
    //printf("5\r\n");
	return 0;
}
