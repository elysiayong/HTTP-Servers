all: SimpleServer.c PersistentServer.c helpers/utils.c helpers/server.c helpers/http.c 
	gcc -Wall -o SimpleServer SimpleServer.c helpers/utils.c helpers/server.c helpers/http.c
	gcc -Wall -o PersistentServer PersistentServer.c helpers/utils.c helpers/server.c helpers/http.c

clean: 
	rm -f SimpleServer PersistentServer