PROGRAM=serviceBright
OBJECTS=main.o rs232.o Rs232Layer.o Protocol.o
FILES=main/main.c comm/rs232.c comm/Rs232Layer.c comm/Protocol.c

CC=gcc
#CFLAGS=$(shell pkg-config --cflags sqlite3)
CFLAGS= -Wall -g

LDFLAGS= #$(shell pkg-config --libs sqlite3)

all: $(PROGRAM)

serviceBright: $(FILES)
	$(CC) $(LDFLAGS) $(FILES) -o $(PROGRAM)

clean:
	rm -rf $(OBJECTS) $(PROGRAM)
	
%.o : %.c
	$(CC) $(CFLAGS) -c $<
