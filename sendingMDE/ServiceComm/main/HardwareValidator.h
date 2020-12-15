#include "../comm/Rs232Layer.h"

union U_Code{
    unsigned char dataByte[8];
    unsigned int dataLong[2];
};

typedef union U_Code Code;

void calculateHash(unsigned char* dataRespuesta, Code* code);
void serviceInitHardwareValidator(void);
void serviceHardwareValidator(Port* port,DbManager *dbm);


