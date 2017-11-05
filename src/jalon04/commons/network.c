/* ******************** functions to init both server and client ****************************** */


#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

#include "functions.h"
#include "network.h"

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
	sockfd = socket(domain, type, protocol);

//check for socket validity
	if(sockfd==-1) {
		error("Socket error");
		return EXIT_FAILURE;
	}

	return sockfd;
}

// int do_socket(int domain, int type, int protocol) {
// 	int sockfd;
// 	int yes = 1;
// //create the socket
// 	sockfd = socket(domain, type, protocol);

// //check for socket validity
// 	if(sockfd==-1) {
// 		fprintf(stderr,"Socket error\n");
// 		return EXIT_FAILURE;
// 	}

// // set socket option, to prevent "already in use" issue when rebooting the server right on

// 	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
// 		error("ERROR setting socket options");

// 	return sockfd;
// }


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

void * readServ(void * arg){
	int *sockfd = arg;
	int maxlen = 500;
	char * msg_s = malloc(sizeof(char) * maxlen);

	for (;;){
		memset(msg_s,0,maxlen);
		//receive msg from server
		readline(*sockfd, msg_s, maxlen);
		printf("\n%s", msg_s);
		fflush(stdout);
	}
	pthread_exit(NULL);
}