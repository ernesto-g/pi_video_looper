#define CONFIG_VIRTUAL 0x03
#define CONFIG_REAL 0x02

#define HARDWARE_SCREEN_0 0x00
#define HARDWARE_SCREEN_1 0x01
#define HARDWARE_SCREEN_2 0x02
#define HARDWARE_SCREEN_3 0x03


#define HARDWARE_SCREEN_CODE_0_H 0x15
#define HARDWARE_SCREEN_CODE_0_L 0x08

#define HARDWARE_SCREEN_CODE_1_H 0xA0
#define HARDWARE_SCREEN_CODE_1_L 0x4B

#define HARDWARE_SCREEN_CODE_2_H 0x00
#define HARDWARE_SCREEN_CODE_2_L 0x00

#define HARDWARE_SCREEN_CODE_3_H 0x74
#define HARDWARE_SCREEN_CODE_3_L 0x68



struct S_packetStart {
    unsigned char head[4];
    unsigned char screenNumber;
    unsigned char packetNumber;
    unsigned char data[19];
    unsigned char crc;
};

struct S_packetBright{
    unsigned char head[4];
    unsigned char screenNumber;
    unsigned char packetNumber;
    unsigned char cmd;
    unsigned char twentyTwo;
    unsigned char address;
    unsigned char data[16];
    unsigned char crc;
};

struct S_dataBright {
    unsigned char realVirtual;
    unsigned char zeros0[7];
    unsigned char rBright;
    unsigned char gBright;
    unsigned char bBright;
    unsigned char ff;
    unsigned char zeros1[4];
};

void sendBrightConfiguration(Port *port,int rBright,int gBright,int bBright,unsigned char realVirtual,unsigned char screenHardwareNumber);

