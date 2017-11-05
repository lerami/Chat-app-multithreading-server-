/*
********
This file contains the headers of channel.c
********
*/

#ifndef _CHANNEL_H_
#define _CHANNEL_H_


struct channel;
struct channel_user;

int newChannel(struct channel** firstChannel, char channel_name[256]);
void destroyChannel(struct channel** firstChannel, char* channel_name) ;
void listChannel(struct channel** firstChannel, char * dest);
int addUserToChannel(struct channel** firstChannel, char* channel_name, int sockfd);
void newChannelUser(struct channel_user** firstChannelUser, int sockfd);
int removeUserFromChannel(struct channel** firstChannel, char* channel_name, int sockfd);
int removeChannelUser(struct channel_user** firstChannelUser, int sockfd);
void getChannelUsers(struct channel** firstChannel, char channel_name[256], int clientSock, int* rcvSock, int* nbUsers);

#endif //_USERS_H_
