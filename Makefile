TARGET: utils client server

utils:
	gcc -g -c serialize/serialize.c -o serialize/serialize.o

client:
	gcc -g -c client.c -o client.o
	gcc -g client.o serialize/serialize.o -o client

server:
	gcc -g -c server.c -o server.o
	gcc -g server.o serialize/serialize.o -o server

clean:
	rm -rf server.o client.o serialize/serialize.o server client
