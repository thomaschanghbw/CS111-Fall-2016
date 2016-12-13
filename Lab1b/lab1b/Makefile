default: server.c client.c
	gcc -lmcrypt -pthread -o server server.c
	gcc -lmcrypt -pthread -o client client.c
server: server.c
	gcc -lmcrypt -pthread -o server server.c
client: client.c
	gcc -lmcrypt -pthread -o client client.c
clean:
	rm -f server client
dist: server.c client.c my.key Makefile README
	tar -czvf lab1b-004632787.tar.gz server.c client.c my.key Makefile README
