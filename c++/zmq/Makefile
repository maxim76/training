BINARY_DIR=./bin/linux

clean:
	$(RM) -f *.o

version: version.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11 -lzmq

client: client.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11 -lzmq

server: server.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11 -lzmq
