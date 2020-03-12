BINARY_DIR=./bin/linux

clean:
	$(RM) -f *.o

#common.o: common.cpp
#	g++ -c common.cpp

connect_test: connect_test.cpp common.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11

dtls_echo_server: dtls_echo_server.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -lssl -lcrypto -std=c++11

dtls_echo_server_simple: dtls_echo_server_simple.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -lssl -lcrypto -std=c++11

dtls_send: dtls_send.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -lssl -lcrypto -std=c++11

getaddrinfo_test: getaddrinfo_test.cpp common.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11

gethostname_test: gethostname_test.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11

getsockname_test: getsockname_test.cpp common.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11

listen_test: listen_test.cpp common.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11

reuseaddr_test: reuseaddr_test.cpp common.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11

tcp_client: tcp_client.cpp common.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11

tcp_server: tcp_server.cpp common.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11

udp_client: udp_client.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11

udp_send: udp_send.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11

udp_server: udp_server.cpp common.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11

udp_connected_client: udp_connected_client.cpp common.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11

udp_connected_client_nonblocking: udp_connected_client_nonblocking.cpp common.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11

udp_connected_server: udp_connected_server.cpp common.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11

udp_server_select: udp_server_select.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11

udp_server_nonblocking: udp_server_nonblocking.cpp common.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11

test_udp_request:	test_udp_request.cpp udp_request.cpp
	g++ -o $(BINARY_DIR)/$@ $^ -std=c++11
