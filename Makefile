all:
	g++ -g -Wall circle_server.cpp Server.cpp Client.cpp messages.cpp -o circle_server

clean:
	rm circle_server

run_server:
	./circle_server $(port)
