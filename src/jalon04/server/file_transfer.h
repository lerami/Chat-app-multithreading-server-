/*
********
This file contains the headers of users.c
********
*/

#ifndef _FILE_TRANSFER_H_rcvrcvrcvrcv
#define _FILE_TRANSFER_H_rcvrcvrcvrcv

struct user;

void strip_arguments(int *socket, char ** filename, char * arguments);
void handle_file_transfer(char * buffer, struct user* users, int client_sock);
void transfer_file(int file_receiver_socket, char* filename, char *buffer);

#endif //_USERS_H_