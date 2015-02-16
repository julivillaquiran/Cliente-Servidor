#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>

int main (void){

    printf ("Connecting to hello world server...\n");
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://127.0.0.1:5555");

	
   
	    char buffer [5];
        zmq_send (requester, "0;2;10;5", 10, 0); 
        zmq_recv (requester, buffer, 5, 0); 
        printf ("Received %s\n",buffer);
    //}
    //while(1){}
    zmq_close (requester); //cerrar los recursos que se estan usando
    zmq_ctx_destroy (context); //cerrar los recursos que se estan usando
    return 0;
}
