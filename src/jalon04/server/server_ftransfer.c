#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "../commons/functions.h"
#include "server_ftransfer.h"
#include "users.h"

void strip_arguments(int *socket, char ** filename, char * arguments){
// get the receiver socket fd and the filename separately out of char * arguments 
}

void handle_file_transfer(char * buffer, struct user* users, int client_sock){

	int file_receiver_socket;
	char * filename;
	int client_msg_size;
	
	strip_arguments(&file_receiver_socket,&filename,buffer+6);
	
	// listen to the answer of file_receiver_socket to the file transfer (ie "Y" or "n")
	do{
		memset(buffer, 0, BUFFER_MAX_SIZE);
		client_msg_size = readline(file_receiver_socket, buffer, BUFFER_MAX_SIZE);
		sprintf(buffer,"%s wants you to accept the transfer of the file named \"%s\". Do you accept? [Y/n]\n", getNickname(users, client_sock), filename);
		sendline(file_receiver_socket,buffer, BUFFER_MAX_SIZE);
	}while((strcmp(buffer,"Y") != 0) || (strcmp(buffer,"n") != 0));

	// treat "Y" answer : send the file
	if (strcmp(buffer,"Y") == 0){
		sprintf(buffer,"%s accepted the file transfer.\n", getNickname(users,file_receiver_socket));
		sendline(client_sock, buffer, BUFFER_MAX_SIZE);
		transfer_file(file_receiver_socket, filename,buffer);
	}

	// treat "n" answer
	else if (strcmp(buffer,"n") == 0){
		sprintf(buffer,"%s cancelled file transfer.\n", getNickname(users,file_receiver_socket));
	}
}


void transfer_file(int file_receiver_socket, char* filename, char *buffer){
// transfer the file filename from client_sock to file_receiver_socket
}