CC=gcc

CFLAGS0=-Wall -c -g -O0

CFLAGS1=-Wall -g -O0

LNAME=unique

.PHONY : clean test cleardata

OBJS=worker.o support.o msg.o htable.o

all: os_server lib$(LNAME).a client tryclient support.o

os_server: os_server.c lib$(LNAME).a macros.h 
	$(CC) $(CFLAGS1) $^ -o $@ -lpthread -L. -l$(LNAME)

lib$(LNAME).a: $(OBJS)
	ar rvs $@ $^

worker.o: worker.c worker.h
	$(CC) $(CFLAGS0) $< -o $@ -lpthread

htable.o: htable.c htable.h
	$(CC) $(CFLAGS0) $< -o $@

msg.o: msg.c msg.h
	$(CC) $(CFLAGS0) $< -o $@

objstore.o: objstore.c objstore.h
	$(CC) $(CFLAGS0) $< -o $@

client: client.c objstore.o support.o
	$(CC) $(CFALGS1) $^ -o $@ 

tryclient: tryclient.c objstore.o support.o
	$(CC) $(CFLAGS1) $^ -o $@

support.o: support.c support.h
	$(CC) $(CFLAGS0) $< -o $@

test: cleardata#prima c'era \@ ./os_server &
	./testscript.sh 1

cleardata:
	@$(RM) -r data/*

clean: 
	-rm *.o os_server lib$(LNAME).a
