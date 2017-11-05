/*
********
client.c : configuration and running of client
jalon 03  : chat system
********
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

#include "../commons/functions.h"
#include "../commons/network.h"


int main(int argc,char** argv)
{

	if (argc != 3)
	{
		fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
		return 1;
	}

	char * IP_addr;
	char * port;
	struct addrinfo* res;
	int maxlen = 500;
	char * msg_c = malloc(sizeof(char) * maxlen);
	char * msg_s = malloc(sizeof(char) * maxlen);
	int s;
	pthread_t nthread;

	memset(&res,0,sizeof(res));

	IP_addr=argv[1];
	port=argv[2];


//get address info from the server
	get_addr_info(IP_addr,port,&res);


//get the socket
	s = do_socket(AF_INET, SOCK_STREAM, 0);

//connect to remote socket
	do_connect(s, res->ai_addr, sizeof(*(res->ai_addr)));


	if(pthread_create(&nthread, NULL, readServ, &s) == -1){
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}
	pthread_detach(nthread);


	for(;;){
		memset(msg_c,0,maxlen);

		//get user input
		//printf("Client :");
		//fflush(stdout);
		readline(STDIN_FILENO, msg_c, maxlen);

		//send msg to server
		sendline(s, msg_c, maxlen);

		//stop connection
		if(strcmp(msg_c,"/quit\n") == 0){
			printf("End of connection\n");
			break;
		}

		//send message to the server
		//handle_client_message()
	}

	close(s);
	freeaddrinfo(res);

	return 0;


}