all:
	g++ -g -Wall circle_server.cpp Server.cpp User.cpp server_messages.cpp -o build/circle_server
	g++ -g -Wall circle_client.cpp Client.cpp client_messages.cpp -o build/circle_client

server:
	g++ -g -Wall circle_server.cpp Server.cpp User.cpp server_messages.cpp -o build/circle_server

client:
	g++ -g -Wall circle_client.cpp Client.cpp client_messages.cpp -o build/circle_client

clean:
	rm build/circle_server
	rm build/circle_client

run_server:
	./build/circle_server $(port)

run_client:
	./build/circle_client
