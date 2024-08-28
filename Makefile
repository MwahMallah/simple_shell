CC = gcc
CFLAGS = -Wall

SRCMODULES = analyzer.c cmd_line.c lexer.c list.c special.c util.c
OBJMODULES = $(SRCMODULES:.c=.o)

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

all: main.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o main

clean:
	rm -rf *.o main *.txt