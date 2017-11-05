/*
********
This file contains the headers of users.c
********
*/

#ifndef _SERVER_FTRANSFER_H_rcvrcvrcvrcv
#define _SERVER_FTRANSFER_H_rcvrcvrcvrcv

struct user;

void strip_arguments(int *socket, char ** filename, char * arguments);
void handle_file_transfer(char * buffer, struct user* users, int client_sock);
void transfer_file(int file_receiver_socket, char* filename, char *buffer);

#endif //_SERVER_FTRANSFER_H_