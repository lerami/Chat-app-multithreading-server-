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

#include "channel.h"

struct channel_user {
  int socket;
  struct channel_user* nextChannelUser;
};


struct channel {
  char channel_name[256];
  int user_nb;
  struct channel_user* channel_user;
  struct channel* nextChannel;

};


int newChannel(struct channel** firstChannel, char channel_name[256]) {
  struct channel* channel = *firstChannel;
  struct channel* temp_chan = *firstChannel;

  while(temp_chan!=NULL){
    if(strcmp(temp_chan->channel_name, channel_name) == 0)
      return -1;
    temp_chan=temp_chan->nextChannel;
  }

  struct channel* newChannel = malloc(sizeof(struct channel));
  struct channel_user *newChannelUser = malloc(sizeof(struct channel_user));

  strcpy(newChannel->channel_name,channel_name);
  newChannel->user_nb=0;
  newChannel->channel_user=NULL;
  newChannel->nextChannel=NULL;

  if(channel==NULL) {
    channel=newChannel;
    *firstChannel=channel;
  }
  else {
      while (channel->nextChannel!=NULL) {
        channel=channel->nextChannel;
      }
      channel->nextChannel=newChannel;
  }

  return 0;
}

void destroyChannel(struct channel** firstChannel, char* channel_name) {
  struct channel* channel=*firstChannel;
  struct channel* prevChannel=NULL;

  if(channel->user_nb > 0){
    perror("This channel is not empty");
    exit(EXIT_FAILURE);
  }

  while(channel!=NULL){
    if(strcmp(channel->channel_name, channel_name) == 0){
      if(prevChannel==NULL){
        *firstChannel=channel->nextChannel;
      }
      else {
        prevChannel->nextChannel=channel->nextChannel;
      }
    }
    prevChannel=channel;
    channel=channel->nextChannel;
  }
}

void listChannel(struct channel** firstChannel, char * dest){
  struct channel* channel;
  char nameBuffer[256];
  int nameLen;
  int msgLen;
  msgLen=25;
  char* msg=malloc(msgLen*sizeof(char));
  strcpy(msg,"Accessible channels are\n");

  channel=*firstChannel;
  while(channel!=NULL){
    nameLen=strlen(channel->channel_name);
    msgLen+=nameLen+1;
    msg=realloc(msg,msgLen*sizeof(char));
    strcat(msg,  channel->channel_name);
    strcat(msg,  "\n");
    channel=channel->nextChannel;
  }

  strcpy(dest, msg);

}

int addUserToChannel(struct channel** firstChannel, char* channel_name, int sockfd){
struct channel* channel = *firstChannel;

  while(channel!=NULL){

    if(strcmp(channel->channel_name,channel_name)==0){
      newChannelUser(&(channel->channel_user),sockfd);
      ++channel->user_nb;
      return 0;
    }
    channel=channel->nextChannel;
  }
  return -1;
//perror("This channel does not exist");

}

void newChannelUser(struct channel_user** firstChannelUser, int sockfd){
  struct channel_user* channel_user = *firstChannelUser;
  struct channel_user* newChannelUser = malloc(sizeof(channel_user));
  newChannelUser->socket=sockfd;
  newChannelUser->nextChannelUser=NULL;

  if(channel_user==NULL){
    channel_user=newChannelUser;
    *firstChannelUser = channel_user;
  }
  else{
    while(channel_user->nextChannelUser!=NULL){
      if(channel_user->socket == sockfd){
        perror("You are already logged in this channel");
        exit(EXIT_FAILURE);
      }
      channel_user=channel_user->nextChannelUser;
    }
    channel_user->nextChannelUser = newChannelUser;
  }
}

int removeUserFromChannel(struct channel**  firstChannel, char* channel_name, int sockfd){
  struct channel * channel = *firstChannel;
  int res;
  while(channel!=NULL){

    if(strcmp(channel->channel_name,channel_name)==0){
      res=removeChannelUser(&(channel->channel_user),sockfd);
      if(res==0){
        --channel->user_nb;
        if(channel->user_nb==0){
          destroyChannel(firstChannel, channel_name);
        }
      }
      return res;
    }
    channel=channel->nextChannel;
  }
  return -1;

}

int removeChannelUser(struct channel_user** firstChannelUser, int sockfd){
  struct channel_user * channel_user = *firstChannelUser;
  struct channel_user * prevChannelUser = NULL;

  while(channel_user!=NULL){
    if(channel_user->socket==sockfd){
      if(prevChannelUser==NULL){
        *firstChannelUser=channel_user->nextChannelUser;
      }
      else {
        prevChannelUser->nextChannelUser=channel_user->nextChannelUser;
      }
      return 0;
    }
    prevChannelUser=channel_user;
    channel_user=channel_user->nextChannelUser;
  }
  return -1;
}

void getChannelUsers(struct channel** firstChannel, char channel_name[256], int clientSock, int* rcvSock, int* nbUsers){
  struct channel* channel = *firstChannel;
  *nbUsers=0;
  while(channel!=NULL){

    if(strcmp(channel->channel_name,channel_name)==0){
      struct channel_user* channelUsers = channel->channel_user;
      while(channelUsers!=NULL){
        if(channelUsers->socket != clientSock){
          *(rcvSock + *nbUsers) = channelUsers->socket;
          *nbUsers = *nbUsers+1;
        }

        channelUsers = channelUsers->nextChannelUser;
      }

    }
    channel=channel->nextChannel;
  }
}
