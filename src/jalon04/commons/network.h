/*
********
This file contains the headers of network.c
********
*/

#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>


void init_serv_addr(const char* port, struct sockaddr_in *serv_addr);
void get_addr_info(const char* address, const char* port, struct addrinfo** res);
int do_socket(int domain, int type, int protocol);
void do_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
void do_bind(int sockfd, struct sockaddr_in serv_addr);
void do_listen(int sockfd, int maxconn);
int do_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen, volatile short int* counter, int maxconn);
void * readServ(void * arg);


#endif //_NETWORK_H_