#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <fcntl.h>
#include <unistd.h>

#ifndef COMMON2015_6_29
void error(const char * str);

int recv_all(int client,char *data,int len);

int send_all(int client,char *data,int len);
#endif
