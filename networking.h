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
#include <curses.h>
#include <math.h>

#ifndef NETWORKING_H
#define NETWORKING_H

#define height 30
#define width 120

//for forking server
int server_setup();
int server_connect(int from_client);

int client_handshake();

#endif
