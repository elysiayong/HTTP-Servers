CFLAGS=-Wall
SRCS = Client.c SimpleServer.c PersistentServer.c PipelinedServer.c
PROGS = $(patsubst %.c, %.o , $(SRCS))

all: $(PROGS)

%.o: %.c
	gcc $(CFLAGS) -o $@ $<

.PHONY: clean

clean : 
	rm -f *.o