OBJS = obj/numpadmouse.o obj/resource.o obj/manageevent.o obj/performaction.o
INCLUDE_DIRS = -I.\inc

WARNS = -Wall

CC = gcc
#LDFLAGS = -s -lcomctl32 -Wl,--subsystem,windows
LDFLAGS = -s -lcomctl32 -lgdi32 -Wl,--subsystem,windows
RC = windres
#CFLAGS= -O3 -std=c99 -D UNICODE -D _UNICODE -D _WIN32_IE=0x0500 -D WINVER=0x500 ${WARNS}
#MYWINVER = 0x0A00
MYWINVER = 0x0500
CFLAGS= -O3 -std=c99 -D UNICODE -D _UNICODE -D _WIN32_IE=0x0500 -D WINVER=${MYWINVER} -D _WIN32_WINNT=${MYWINVER} -D _WIN32_WINDOWS=${MYWINVER} ${WARNS}

all: clean numpadmouse.exe

numpadmouse.exe: ${OBJS}
	${CC} -o "$@" ${OBJS} ${LDFLAGS}

clean:
	del obj\*.o 
	del numpadmouse.exe

obj/%.o: src/%.c ${HEADERS}
	${CC} ${CFLAGS} ${INCLUDE_DIRS} -c $< -o $@

obj/resource.o: res/resource.rc res/application.manifest res/on.ico res/off.ico inc/resource.h
	${RC} ${INCLUDE_DIRS} -I.\res -i $< -o $@
