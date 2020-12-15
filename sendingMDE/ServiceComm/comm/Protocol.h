
#ifndef PROTOCOL_H
#define	PROTOCOL_H


#define CMD_SENSORS_DATA 0x00
#define CMD_SET_DATETIME 0x01
#define CMD_HARD_VALIDATE 0x02
#define CMD_SET_USER_VAR 0x03


struct S_packet
{
    char cmd;
    char *pData;
	int len;
};

typedef struct S_packet Packet;

struct S_BufferUart
{
    char* pData;
    int len;
};

typedef struct S_BufferUart BufferUart;

int protocol_analizePacket(unsigned char * buff, int size, Packet * packet);
int protocol_SplitBuffer(char* buffRx,int len, BufferUart* buffers,int buffersLen);
int protocol_packData(Packet* packet,char* buffer);

#endif


