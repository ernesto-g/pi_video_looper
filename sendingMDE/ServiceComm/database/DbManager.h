
#ifndef DBMANAGER_H
#define	DBMANAGER_H

#include "sqlite3.h"

struct S_DbManager
{
	sqlite3 * db;
};

typedef struct S_DbManager DbManager;

int dbm_openDb(DbManager * dbManager,char * path,int flags);
int dbm_closeDb(DbManager * dbManager);

#endif


