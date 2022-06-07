all:
	gcc -g -Wall -lm connection.c server.c -o server
	gcc -g -Wall -lm connection.c client.c -o client
