#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <math.h>

#ifndef NETWORKING_H
#define NETWORKING_H

#define height 20
#define width 80
#define radius 10
#define namelen 20
#define gametime 80
#define LOGIN 0
#define CREATE 1

//for forking server
int server_setup();
int server_connect(int from_client);
int client_handshake();
void writeint(int fd, int x);

void load_usernames();
void add_username(char * line);
int user_exists(char * line);
int check_username(char * line, int n);

#endif
