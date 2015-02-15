CC=g++
ZMQ=/julianv/Home/zmq
ZMQ_LIB=$(ZMQ)/lib
ZMQ_HDR=$(ZMQ)/include

all: server client

server: server.cc
	$(CC) -I$(ZMQ_HDR) -L$(ZMQ_LIB) -o server server.cc -lzmq

client: client.cc
	$(CC) -I$(ZMQ_HDR) -L$(ZMQ_LIB) -o client client.cc -lzmq
    
clean:
	rm -f client server *~
