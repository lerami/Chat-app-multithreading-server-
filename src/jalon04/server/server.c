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

#include "../commons/functions.h"
#include "users.h"
#include "channel.h"
#include "file_transfer.h"

struct arg_threads {
	int* sockfd;
	struct user** users;
	struct channel ** channels;
	struct sockaddr_in cl_addr;
};

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
	char * buffer = malloc(sizeof(char) * BUFFER_MAX_SIZE);
	int client_msg_size;
	int msglen;
	int rcvSock[20];
	int nbSock;
	char clientChan[256];
	char error_msg[BUFFER_MAX_SIZE];
	char whoisBuff[40];
	char nickBuff[260];
	char msgBuff[510];
	char msgallBuff[510];
	char sendBuff[510];
	char createChannelBuff[100];
	char joinChannelBuff[100];
	char quitChannelBuff[100];

	struct arg_threads* arg_threads=arg;
	struct user** users = arg_threads->users;
	struct channel** channels = arg_threads->channels;
	int client_sock = *(arg_threads->sockfd);
	struct sockaddr_in cl_addr = arg_threads->cl_addr;


	printf("Connection accepted \n");
	printf("client_sock : %i \n", client_sock);

  // Nickname
	do{
		sendline(client_sock, "[Server]: Please logon with /nick <your pseudo>\n", 48);
		memset(buffer,0,BUFFER_MAX_SIZE);
    //read what the client has to say
		msglen = readline(client_sock, buffer, BUFFER_MAX_SIZE);
		buffer[5]='\0';
		if(strcmp(buffer,"/nick") == 0){
			stripLine(buffer, 6);
			int res = addUser(users,buffer+6,cl_addr,client_sock, error_msg);
			if(res==0){
				char new_msg[30+strlen(buffer+6)];
				sprintf(new_msg, "[Server]: Welcome to the chat %s\n", buffer+6);
				sendline(client_sock, new_msg, strlen(new_msg));
				break;
			}
			else if(res==-1){
				sendline(client_sock, error_msg, strlen(error_msg));
			}
		}
	}while(strcmp(buffer, "/quit\n"));


	do{
		memset(buffer, 0, BUFFER_MAX_SIZE);
		client_msg_size = readline(client_sock, buffer, BUFFER_MAX_SIZE);
		rcvSock[0]=client_sock;
		nbSock=1;

		if(buffer[0]!='/'){
			char newBuff[BUFFER_MAX_SIZE];
			strcpy(newBuff, buffer);
			sprintf(buffer, "[Server]: %s",newBuff);
		}

		if(strcmp(buffer,"/who\n") == 0){
			listUsers(*users, buffer);
		}

		strcpy(whoisBuff,buffer);
		whoisBuff[6]='\0';
		if(strcmp(whoisBuff,"/whois") == 0){
			stripLine(buffer, 7);
			infoUser(*users, buffer+7, buffer);
		}

		strcpy(nickBuff,buffer);
		nickBuff[5]='\0';
		if(strcmp(nickBuff,"/nick") == 0){
			stripLine(nickBuff, 6);
			changeNickname(users, client_sock, nickBuff+6);
			sprintf(buffer, "[Server]: Username was changed to %s\n", nickBuff+6);
		}

		strcpy(msgallBuff,buffer);
		msgallBuff[7]='\0';
		if(strcmp(msgallBuff,"/msgall") == 0){
			listSockets(*users, rcvSock, &nbSock, client_sock);
			if(nbSock == 0){
				sprintf(buffer, "[Server]: Nobody else connected!");
				nbSock = 1;
				rcvSock[0]=client_sock;
			}
			else{
				sprintf(buffer,"%i : %s", client_sock, msgallBuff+8);
			}
		}

		strcpy(msgBuff,buffer);
		msgBuff[4]='\0';
		if(strcmp(msgBuff,"/msg") == 0){
			int c=stripSpace(msgBuff, 5);
			rcvSock[0]=getSocket(*users, msgBuff+5 );
			if(rcvSock[0]>0){
				sprintf(buffer, "[%s]: %s\n", getNickname(*users, client_sock), msgBuff+c+1);
			}
			else {
				rcvSock[0]=client_sock;
				sprintf(buffer, "[Server]: No user under this username");
			}
		}

		if(strcmp(buffer,"/list\n") == 0){
			listChannel(channels, buffer);
		}

		strcpy(createChannelBuff, buffer);
		createChannelBuff[7]='\0';
		if(strcmp(createChannelBuff,"/create") == 0){
			stripLine(createChannelBuff, 8);
			newChannel(channels, createChannelBuff+8);
			rcvSock[0]=client_sock;
			sprintf(buffer, "[Server]: You have created channel %s\n", createChannelBuff+8);
		}

		strcpy(joinChannelBuff, buffer);
		joinChannelBuff[5]='\0';
		if(strcmp(joinChannelBuff,"/join") == 0){
			stripLine(joinChannelBuff, 6);
			addUserToChannel(channels, joinChannelBuff+6, client_sock);
			rcvSock[0]=client_sock;
			sprintf(buffer, "[Server]: You have joined channel %s\n", joinChannelBuff+6);
		}


		strcpy(sendBuff,buffer);
		sendBuff[5]='\0';
		if(strcmp(sendBuff,"/send") == 0){
			handle_file_transfer(buffer, *users, client_sock);
			rcvSock[0] = client_sock;
		}

		int i;
		for(i=0;i<nbSock;i++){
			sendline(rcvSock[i], buffer, BUFFER_MAX_SIZE);
		}

	}while(strcmp(buffer, "/quit\n"));



	removeUser(users, client_sock);

        //clean up client socket
	close(client_sock);
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
	MAXCONN = 2;
	do_listen(sockfd, MAXCONN);

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
