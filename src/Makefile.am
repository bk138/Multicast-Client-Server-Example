## Process this file with automake to produce Makefile.in

AM_CFLAGS =\
	 -Wall \
	 -g

bin_PROGRAMS = server client

server_SOURCES = server.c msock.c
client_SOURCES = client.c msock.c

LDADD = @WSOCKLIB@
