void ssm_init(void);
int ssm_getClientLen(void);
void ssm_closeAll(void);
void ssm_poolClientsPackets(void (*callbackReleasePacket)(char*,int));
void ssm_sendPacketToAllClients(char* pData,int len);

