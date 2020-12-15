PROGRAM=serviceComm
OBJECTS=main.o rs232.o Rs232Layer.o DbManager.o Protocol.o VarDAO.o ServiceLogic.o SysDAO.o HardwareValidator.o SocketServer.o SocketServerManager.o
FILES=main/main.c main/ServiceLogic.c main/HardwareValidator.c comm/rs232.c comm/Rs232Layer.c comm/Protocol.c database/DbManager.c database/VarDAO.c database/SysDAO.c comm/SocketServer.c comm/SocketServerManager.c

CC=gcc
CFLAGS=$(shell pkg-config --cflags sqlite3)
CFLAGS+= -Wall -g

LDFLAGS=$(shell pkg-config --libs sqlite3) -lpthread

all: $(PROGRAM)

serviceComm: $(FILES)
	$(CC) $(LDFLAGS) $(FILES) -o $(PROGRAM)

clean:
	rm -rf $(OBJECTS) $(PROGRAM)

%.o : %.c
	$(CC) $(CFLAGS) -c $<
