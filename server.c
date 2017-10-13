/*
********
server.c : configuration & running of the server
jalon 02 : chat system
********
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "functions.h"

// Initialization global variables
volatile short int connected_nb;  
pthread_mutex_t mut;

int do_socket(int domain, int type, int protocol) {
	int sockfd;
	sockfd = socket(domain, type, protocol);

//check for socket validity
	if(sockfd==-1) {
		error("Socket error");
		return EXIT_FAILURE;
	}

	return sockfd;
}

void init_serv_addr(const char* port, struct sockaddr_in *serv_addr) {

	int portno;

//clean the serv_add structure
	memset(serv_addr, 0, sizeof(*serv_addr));
//cast the port from a string to an int
	portno   =  atoi(port);
//internet family protocol
	serv_addr->sin_family =AF_INET;
//we bind to any ip form the host
	serv_addr->sin_addr.s_addr =INADDR_ANY;
//we bind on the tcp port specified
	serv_addr->sin_port = htons(portno);
}

void do_bind(int sockfd, struct sockaddr_in serv_addr) {
	if( bind( sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		error("Binding error");
	}
}

void do_listen(int sockfd, int maxconn) {
	if(listen(sockfd, maxconn)==-1) {
		error("Listen error");
	}
}

int do_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen, volatile short int* counter, int maxconn){
	int c_sock;

	c_sock=accept(sockfd, addr, addrlen);
	if(c_sock==-1) {
		error("Accept error");
	}

	if (*counter<maxconn){
    write(c_sock,"1",1); // Tell the client his connection has succeded
    return c_sock;
}
else {
    write(c_sock,"0",1);  // Tell the client his connection was refused because there were too many connections
    close(c_sock);
    return -1;
}

}

void * start_routine_new_client(void * arg){
	int BUFFER_MAX_SIZE = 500;
	char * buffer = malloc(sizeof(char) * BUFFER_MAX_SIZE);
	int client_msg_size;
	int * client_sock = (int *) arg;

	printf("Connection accepted \n");
	printf("client_sock : %i \n", *client_sock);

	for(;;){

		do{memset(buffer, 0, BUFFER_MAX_SIZE);
			client_msg_size = readline(* client_sock, buffer, BUFFER_MAX_SIZE);
			printf("message %s\n", buffer);
			sendline(* client_sock, buffer, BUFFER_MAX_SIZE);
		}while(strcmp(buffer, "/quit\n"));
		
	}

        //clean up client socket
	close(* client_sock);
	pthread_mutex_lock(&mut);
	connected_nb --;
	pthread_mutex_unlock(&mut);

	pthread_exit(NULL);
}


int main(int argc, char** argv)
{

	if (argc != 2)
	{
		fprintf(stderr, "usage: RE216_SERVER port\n");
		return 1;
	}

	int sockfd;
	const char* port;
	struct sockaddr_in serv_addr;
	int MAXCONN;

	connected_nb=0;
	port = argv[1];

  //create the socket, check for validity!
	sockfd = do_socket(AF_INET,SOCK_STREAM,0);


  //init the serv_add structure
	init_serv_addr(port, &serv_addr);

  //perform the binding
  //we bind on the tcp port specified
	do_bind(sockfd, serv_addr);

  //specify the socket to be a server socket and listen for at most 20 concurrent client
	MAXCONN = 2;
	do_listen(sockfd, MAXCONN);

	int * client_sock;
	struct sockaddr cl_addr;
	socklen_t cl_addrlen = sizeof(cl_addr);
	pthread_t nthread; 

	pthread_mutex_init(&mut, NULL);
	pthread_mutex_lock(&mut);
	connected_nb = 0;
	pthread_mutex_unlock(&mut);

	for(;;){
              //accept connection from client
		client_sock = malloc(sizeof(int));

		*  client_sock=do_accept(sockfd, &cl_addr, &cl_addrlen, &connected_nb, MAXCONN);

		if (connected_nb < MAXCONN){

			pthread_mutex_lock(&mut);
			connected_nb ++;
			pthread_mutex_unlock(&mut);

			if(pthread_create(&nthread, NULL, start_routine_new_client, client_sock) == -1){
				perror("pthread_create");
				exit(EXIT_FAILURE);
			}
			pthread_detach(nthread);
		}
		else{
			close(* client_sock);
		}
	}
  //clean up server socket
	close(sockfd);
	pthread_mutex_destroy(&mut);

	return 0;
}
