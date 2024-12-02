CC=gcc
CFLAGS=-I.
DEPS_S = serverheaders.h chatserver.c serverclientqueue.c serverhandlers.c serverhelpers.c serverinit.c serveruserlist.c
DEPS_C = clientheaders.h chatclient.c clienthelpers.c clientinit.c clientmessagelist.c

all: chatserver chatclient

chatserver: $(DEPS_S)
	$(CC) -o $@ $^ $(CFLAGS)

chatclient: $(DEPS_S)
	$(CC) -o $@ $^ $(CFLAGS)