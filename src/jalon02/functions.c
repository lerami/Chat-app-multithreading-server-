/*
********
This file contains the functions used in client.c and server.c 
********
*/


#include "functions.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

void error(const char *msg)
{
	perror(msg);
	exit(1);
}


ssize_t readline(int fd, char *str, ssize_t maxlen){

	int written = 0;
	ssize_t status;
	do{
		status = read(fd, str + written, maxlen - written);
		written=status + written;
	}while((status>0) && (str[written-1]!='\n'));
	return written;
}


ssize_t sendline(int fd, const char *str, ssize_t maxlen){

	int sent=0;

	do{
		sent += write(fd, str + sent, maxlen-sent);
	}while(sent != maxlen);

	return sent;
}