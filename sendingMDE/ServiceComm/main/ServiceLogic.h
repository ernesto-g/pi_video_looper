
#include "../database/DbManager.h"
#include "../comm/Rs232Layer.h"
#include "../comm/Protocol.h"

void setDateTime(DbManager * dbm,int yr,int mon,int day,int hr,int min,int sec);

void runNTP(Port* port,DbManager *dbm);

int serviceLogic(Port *port,DbManager *dbm, Packet *packet);

void serviceDateTimeManager(Port* port,DbManager* dbm);

