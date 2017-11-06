
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

char resize_buffer(char * buffer){
  char * res = malloc(sizeof(char));
  int i = 0;

  do {
    *(char+i) = buffer[i];
    i++;
  } while (buffer[i] != '\0')

  return res;
}

void * start_routine_new_client(void * arg){
	char * buffer = malloc(sizeof(char) * BUFFER_MAX_SIZE);
	int client_msg_size;
	int msglen;
  int rcvSock[20];
  int nbSock;
  int clientSent;
  char clientChan[256];
  char error_msg[BUFFER_MAX_SIZE];
  char userEntry[124];
	char whoisBuff[40];
	char nickBuff[260];
  char msgBuff[510];
  char msgallBuff[510];
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

  strcpy(clientChan,"");

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
        char* new_msg=malloc(sizeof(char)*(35+2*strlen(buffer+6)));
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
    clientSent=0;

    if(buffer[0]!='/'){
      char newBuff[BUFFER_MAX_SIZE];
      if(strcmp(clientChan,"")==0){
        strcpy(newBuff, buffer);
        sprintf(buffer, "[Server]: %s",newBuff);
      }
      else {
        getChannelUsers(channels, clientChan, client_sock, rcvSock, &nbSock);
        strcpy(newBuff, buffer);
        sprintf(buffer, "[%s]: %s",getNickname(*users, client_sock),newBuff);

      }
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
        sprintf(buffer,"%s : %s", getNickname(*users, client_sock), msgallBuff+8);
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
      if(newChannel(channels, createChannelBuff+8)==0){
        sprintf(buffer, "[Server]: You have created channel %s\n", createChannelBuff+8);
      }
      else{
        sprintf(buffer, "[Server]: Channel name not available, please choose another one\n");
      }
    }

    strcpy(joinChannelBuff, buffer);
    joinChannelBuff[5]='\0';
    if(strcmp(joinChannelBuff,"/join") == 0){
      stripLine(joinChannelBuff, 6);
      if(addUserToChannel(channels, joinChannelBuff+6, client_sock)==0){
        sprintf(buffer, "[Server]: You have joined channel %s\n", joinChannelBuff+6);
        strcpy(clientChan, joinChannelBuff+6);
      }
      else{
        sprintf(buffer, "[Server]: Channel %s not found\n", joinChannelBuff+6);
      }

    }

    strcpy(quitChannelBuff, buffer);
    quitChannelBuff[6]='\0';
    if(strcmp(quitChannelBuff,"/leave") == 0 && strcmp(buffer,"/leave\n") != 0){
      stripLine(quitChannelBuff, 7);
      if(removeUserFromChannel(channels, quitChannelBuff+7, client_sock)==0){
        strcpy(clientChan, "");
        sprintf(buffer, "[Server]: You have exited channel %s\n", quitChannelBuff+7);
      }
      else{
        sprintf(buffer, "[Server]: Error trying to quit channel\n");
      }
    }

    // strcpy(sendBuff,buffer);
    // sendBuff[5]='\0';
    // if(strcmp(sendBuff,"/send") == 0){
    // 	handle_file_transfer(buffer, *users, client_sock);
    // 	rcvSock[0] = client_sock;
    // }

    int i;
    for(i=0;i<nbSock;i++){

      if(client_sock==rcvSock[i])
        clientSent=1;

      if(strcmp(clientChan,"")) {
        sprintf(userEntry,"\n[%s][%s]:",getNickname(*users, rcvSock[i]), clientChan);
      }
      else {
        sprintf(userEntry,"\n[%s]:",getNickname(*users, rcvSock[i]));
      }
      strcat(buffer,userEntry);
		  sendline(rcvSock[i], buffer, BUFFER_MAX_SIZE);
    }
    if(clientSent==0){
      if(strcmp(clientChan,"")) {
        sprintf(userEntry,"\n[%s][%s]:",getNickname(*users, client_sock), clientChan);
      }
      else {
        sprintf(userEntry,"\n[%s]:",getNickname(*users, client_sock));
      }
      sendline(client_sock, userEntry, BUFFER_MAX_SIZE);

    }

	}while(strcmp(buffer, "/quit\n"));

  if(strcmp(clientChan,"")){
    removeUserFromChannel(channels, clientChan, client_sock);
  }

	removeUser(users, client_sock);

        //clean up client socket
	close(client_sock);
	pthread_mutex_lock(&mut);
		connected_nb --;
	pthread_mutex_unlock(&mut);

//	pthread_exit(NULL);
}