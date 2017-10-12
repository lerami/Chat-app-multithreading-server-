/*
********
client.c : configuration and running of client
jalon 02  : chat system
********
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "functions.h"

void get_addr_info(const char* address, const char* port, struct addrinfo** res) {

	int status;
	struct addrinfo hints, p;

	memset(&hints,0,sizeof(hints));

	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_STREAM;

	hints.ai_flags = 0;
	hints.ai_protocol = 0;


  //  hints.ai_family =
  //  hints.ai_socktype =

	status = getaddrinfo(address,port,&hints,res);
	if (status != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		exit(EXIT_FAILURE);
	}

}

int do_socket(int domain, int type, int protocol) {
	int sockfd;
	int yes = 1;
//create the socket
	sockfd = socket(domain, type, protocol);

//check for socket validity
	if(sockfd==-1) {
		fprintf(stderr,"Socket error\n");
		return EXIT_FAILURE;
	}

// set socket option, to prevent "already in use" issue when rebooting the server right on

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		error("ERROR setting socket options");

	return sockfd;
}

void do_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {

	int res = connect(sockfd, addr, addrlen);
	if (res != 0) {
		error("Connection error\n");
	}

	char buffer[1];
	read(sockfd, buffer, 1);

	if(strcmp(buffer,"0")==0) {
		printf(" Server cannot accept incoming connections anymore. Try again later.\n");
		close(sockfd);
		exit(0);
	}
}


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

	memset(&res,0,sizeof(res));

	IP_addr=argv[1];
	port=argv[2];


//get address info from the server
	get_addr_info(IP_addr,port,&res);


//get the socket
	s = do_socket(AF_INET, SOCK_STREAM, 0);

//connect to remote socket
	do_connect(s, res->ai_addr, sizeof(*(res->ai_addr)));

	for(;;){
		memset(msg_c,0,maxlen);
		memset(msg_s,0,maxlen);

      //get user input
		printf("Client :");
		fflush(stdout);
		readline(STDIN_FILENO, msg_c, maxlen);

      //send msg to server
		sendline(s, msg_c, maxlen);

      //stop connection
		if(strcmp(msg_c,"/quit\n") == 0){
			printf("End of connection\n");
			break;
		}

      //receive msg from server
		readline(s, msg_s, maxlen);
		printf("Server : %s \n", msg_s);

      //send message to the server
      //handle_client_message()
	}

	close(s);
	freeaddrinfo(res);

	return 0;


}
