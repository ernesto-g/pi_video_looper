#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "DbManager.h"
#include "SysDAO.h"

int sys_setPendingReboot(DbManager* dbm)
{
    sqlite3_stmt * stm;
    sqlite3 * db = dbm->db;

    char sentence[256];
    sprintf(sentence,"UPDATE Reboot SET flagPendingReboot=1 \0");
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


