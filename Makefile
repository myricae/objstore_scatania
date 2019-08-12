CC=gcc

CFLAGS0=-Wall -c

CFLAGS1=-Wall -g -fsanitize=address

LNAME=unique

.PHONY : clean

OBJS=worker.o support.o msg.o htable.o

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

support.o: support.c support.h
	$(CC) $(CFLAGS0) $< -o $@

clean: 
	-rm *.o os_server lib$(LNAME).a