#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <math.h>
#include <signal.h>

#ifndef NETWORKING_H
#define NETWORKING_H

#define SERVERPORT "18168"

#define height 24
#define width 80
#define radius 8
#define namelen 20
#define gametime 69
#define LOGIN 0
#define CREATE 1

//for forking server
int server_setup();
int server_connect(int from_client);
int client_handshake(char* ip);
void writeint(int fd, int x);

void load_usernames();
void add_username(char * line);
int user_exists(char * line);
int check_username(char * line, int n);

// for past games
struct past_game {
	struct tm date;
	int role;
	int result;
};

struct account {
	char username[21];
	int numgames;
	struct past_game * history;
};

#endif
