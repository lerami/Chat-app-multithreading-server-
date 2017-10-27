/*
********
This file contains the headers of channel.c
********
*/

#ifndef _CHANNEL_H_
#define _CHANNEL_H_


struct channel;
struct channel_user;

void newChannel(struct channel** firstChannel, char channel_name[256]);
void destroyChannel(struct channel** firstChannel, char* channel_name) ;
void listChannel(struct channel** firstChannel, char * dest);
void addUserToChannel(struct channel** firstChannel, char* channel_name, int sockfd);
void newChannelUser(struct channel_user** firstChannelUser, int sockfd);
void removeUserFromChannel(struct channel** firstChannel, char* channel_name, int sockfd);
void removeChannelUser(struct channel_user** firstChannelUser, int sockfd);

#endif //_USERS_H_
