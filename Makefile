CC    = gcc
INCS  = -I/usr/include
LIBS  = -L/usr/lib
LINKS = -lc -lm

WARNINGS = -Wall -Wextra -Wshadow -Wpointer-arith \
						-Wcast-align -Wwrite-strings -Wmissing-prototypes \
						-Wmissing-declarations -Wredundant-decls -Wnested-externs \
						-Winline -Wuninitialized -Wconversion -Wstrict-prototypes \
						-pedantic -ansi

CFLAGS = -std=c89 $(WARNINGS) -g

all: EXE 
	
EXE:
	$(CC) $(CFLAGS) src/main.c -c
	$(CC) $(CFLAGS) src/mathcompiler.c -c
	$(CC) $(CFLAGS) src/commands.c -c
	$(CC) $(INCS) $(LIBS) $(LINKS) main.o mathcompiler.o commands.o -o mcc
