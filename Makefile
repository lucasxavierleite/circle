all:
	gcc connection.c server.c -o server
	gcc connection.c client.c -o client
