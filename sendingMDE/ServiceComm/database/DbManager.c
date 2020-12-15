#include "sqlite3.h"
#include "DbManager.h"

//static DbManager dbManager;

int dbm_openDb(DbManager * dbManager,char * path,int flags)
{

	sqlite3 *db;
	int rc = sqlite3_open_v2(path, &db,SQLITE_OPEN_READWRITE,0);
	if(rc)
	{
		if(db!=0)
	    		sqlite3_close(db);
		return -1;
	}

	dbManager->db = db;

	return 0;
}

int dbm_closeDb(DbManager * dbManager)
{

	if(sqlite3_close(dbManager->db)!=SQLITE_OK)
	{
		return -1;
	}

	return 0;
}

