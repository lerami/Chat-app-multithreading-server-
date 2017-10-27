
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#include "users.h"


struct user {
  char nickname[256];
  time_t connectTime;
  struct sockaddr_in address;
  int sockfd;
  struct user* nextUser;

};

int addUser(struct user** firstUser, char nickname[256], struct sockaddr_in address, int sockfd, char* error_msg) {
  struct user* user = *firstUser;

  int i;
  for(i=0;i<strlen(nickname);i++){
    if(nickname[i]==' '){
      strcpy(error_msg,"[Server: ] You can't have a blank space in your username\n");
      return -1;
    }
  }

  struct user* newUser = malloc(sizeof(struct user));
  strcpy(newUser->nickname,nickname);
  newUser->connectTime=time(NULL);
  newUser->address=address;
  newUser->sockfd=sockfd;
  newUser->nextUser=NULL;

  if(user==NULL) {
    user=newUser;
    *firstUser=user;
  }
  else {
      while (user->nextUser!=NULL) {
        if(strcmp(user->nickname, nickname)==0){
          free(newUser);
          strcpy(error_msg,"[Server: ] Username is already taken\n");
          return -1;
        }
        user=user->nextUser;
      }
      if(strcmp(user->nickname, nickname)==0){
        free(newUser);
        strcpy(error_msg,"[Server: ] Username is already taken\n");
        return -1;
      }
      user->nextUser=newUser;
  }

  return 0;
}

void removeUser(struct user** firstUser, int sockfd) {
  struct user* user=*firstUser;
  struct user* prevUser=NULL;

  while(user!=NULL){
    if(user->sockfd==sockfd){
      if(prevUser==NULL){
        *firstUser=user->nextUser;
      }
      else {
        prevUser->nextUser=user->nextUser;
      }
    }
    prevUser=user;
    user=user->nextUser;
  }
}

void changeNickname(struct user** firstUser, int sockfd, char nickname[256]){
  struct user* user=*firstUser;

  while(user!=NULL){
    if(user->sockfd==sockfd){
        strcpy(user->nickname, nickname);
    }
    user=user->nextUser;
  }
}

void listUsers(struct user* firstUser, char* dest){
  struct user* user;
  char nickBuffer[256];
  int nickLen;
  int msgLen;
  msgLen=27;
  char* msg=malloc(msgLen*sizeof(char));
  strcpy(msg,"[Server]: Online users are\n");

  user=firstUser;
  while(user!=NULL){
    nickLen=strlen(user->nickname);
    msgLen+=nickLen+1;
    msg=realloc(msg,msgLen*sizeof(char));
    strcat(msg,  user->nickname);
    strcat(msg,  "\n");
    user=user->nextUser;
  }

  strcpy(dest, msg);

}

void infoUser(struct user* firstUser, char nickname[256], char* dest){

    struct user* user=firstUser;
    int msgLen;
    char *msg;
    while(user!=NULL){
      if(strcmp(user->nickname, nickname)==0){

            char* c_time_string = ctime(&(user->connectTime));
            char* address_string = inet_ntoa(user->address.sin_addr);
            msgLen=strlen(nickname) + strlen(c_time_string) + strlen(address_string)+70;
            msg=malloc(msgLen*sizeof(char));
            sprintf(msg,"[Server]: %s connected since %s with  IP address %s and port number %d\n", nickname, c_time_string, address_string, user->address.sin_port);
            strcpy(dest,msg);
            return;
      }

      user=user->nextUser;
    }

    strcpy(dest,"[Server]: No user found under this pseudo\n");
}

char* getNickname(struct user* firstUser, int sockfd){
  struct user* user =firstUser;
  while(user!=NULL){
    if(user->sockfd == sockfd){
      return user->nickname;
    }
    user=user->nextUser;
  }
  return NULL;
}

int getSocket(struct user* firstUser, char nickname[256]){

  struct user* user =firstUser;
  while(user!=NULL){
    if(strcmp(user->nickname, nickname)==0){
      return user->sockfd;
    }
    user=user->nextUser;
  }
  return -1;
}

void listSockets(struct user* firstUser, int* dest, int* len, int sender_sockfd){
  struct user* user;
  int i = 0;

  user=firstUser;
  while(user!=NULL){
    if(user->sockfd != sender_sockfd){
      *(dest + i) = user->sockfd;
      i++;
     }
      user=user->nextUser;
  }

  *len = i;

}
