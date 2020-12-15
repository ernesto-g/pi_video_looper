PROGRAM=serviceWatchdog
OBJECTS=main.o  SettingsDAO.o DbManager.o
FILES=main.c SettingsDAO.c DbManager.c

CC=gcc
CFLAGS=$(shell pkg-config --cflags sqlite3)
CFLAGS+= -Wall -g

LDFLAGS=$(shell pkg-config --libs sqlite3)

all: $(PROGRAM)

serviceWatchdog: $(FILES)
	$(CC) $(LDFLAGS) $(FILES) -o $(PROGRAM)

clean:
	rm -rf $(OBJECTS) $(PROGRAM)
	
%.o : %.c
	$(CC) $(CFLAGS) -c $<
