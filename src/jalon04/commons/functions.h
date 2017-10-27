/*
********
This file contains the headers of functions.c
********
*/

#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <stdio.h>
#include <stdlib.h>

#define BUFFER_MAX_SIZE 500

void error(const char *msg);
ssize_t readline(int fd, char *str, ssize_t maxlen);
ssize_t sendline(int fd, const char *str, ssize_t maxlen);
int stripLine(char* buffer, int c);
int stripSpace(char* buffer, int c);

#endif //_FUNCTIONS_H_
