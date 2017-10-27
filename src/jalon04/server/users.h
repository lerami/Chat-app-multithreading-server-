/*
********
This file contains the headers of users.c
********
*/

#ifndef _USERS_H_
#define _USERS_H_


struct user;

int addUser(struct user** firstUser, char nickname[256], struct sockaddr_in address, int sockfd, char* error_msg);
void removeUser(struct user** firstUser, int sockfd);
void changeNickname(struct user** firstUser, int sockfd, char nickname[256]);
void listUsers(struct user* firstUser, char* dest);
void infoUser(struct user* firstUser, char nickname[256], char* dest);
char* getNickname(struct user* firstUser, int sockfd);
int getSocket(struct user* firstUser, char nickname[256]);
void listSockets(struct user* firstUser, int* dest, int* len, int sender_sockfd);

#endif //_USERS_H_
