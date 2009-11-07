
## 


CFLAGS=-O2 -g

#LIBS = $(shell $(WXCONFIG) --libs net,adv) 

#
#
#


#
#

.PHONY : clean 

all: client server

client:	 client.o
	$(CC) client.o -o client $(LIBS)

server:	 server.o
	$(CC) server.o -o server $(LIBS)



# clean me up, scotty
clean:
	$(RM) $(OUTFILE) $(OBJS)  *~ 

