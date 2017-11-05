/*
********
This file contains the headers of server_lib.c
********
*/

#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include "users.h"
#include "channel.h"

struct arg_threads;

// Initialization global variables
volatile short int connected_nb;
pthread_mutex_t mut;

void * start_routine_new_client(void * arg);

#endif //_SERVER_H_