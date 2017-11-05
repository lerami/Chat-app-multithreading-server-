/*
********
server.c : configuration & running of the server
jalon 04 : chat system
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

#include "server.h"
#include "../commons/functions.h"
#include "../commons/network.h"
#include "users.h"
#include "channel.h"

struct arg_threads {
  int* sockfd;
  struct user** users;
  struct channel ** channels;
  struct sockaddr_in cl_addr;
};

// Initialization global variables
volatile short int connected_nb;
pthread_mutex_t mut;


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
	struct user* users=NULL;
  	struct channel* channels=NULL;

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
	MAXCONN = 5;
	do_listen(sockfd, MAXCONN);

	//accept_new_connection_process();

	int * client_sock;
	struct sockaddr_in cl_addr;
	socklen_t cl_addrlen = sizeof(cl_addr);
	pthread_t nthread;

	pthread_mutex_init(&mut, NULL);
	pthread_mutex_lock(&mut);
		connected_nb = 0;
	pthread_mutex_unlock(&mut);

	for(;;){
              //accept connection from client
		client_sock = malloc(sizeof(int));

		*client_sock=do_accept(sockfd, (struct sockaddr *) &cl_addr, &cl_addrlen, &connected_nb, MAXCONN);

		if (connected_nb < MAXCONN){

			pthread_mutex_lock(&mut);
				connected_nb ++;
			pthread_mutex_unlock(&mut);

			struct arg_threads* arg_threads = malloc(sizeof(struct arg_threads));
			arg_threads->sockfd=client_sock;
			arg_threads->users=&users;
      arg_threads->channels=&channels;
			arg_threads->cl_addr=cl_addr;

			if(pthread_create(&nthread, NULL, start_routine_new_client, arg_threads) == -1){
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