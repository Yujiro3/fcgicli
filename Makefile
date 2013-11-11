PROGRAM = fcgicli
CC = g++
CFLAGS = -Wall
SRCS = main.cpp fcgi_client.cpp

all:
	$(CC) $(CFLAGS) -o $(PROGRAM) $(SRCS)

clean:
	rm $(PROGRAM)
