#include "common.h"

void error(const char * str){
  perror(str);
  exit(1);
}

int recv_all(int client,char *data,int len){
  int r=0,n=0;
  while(r<len){
    n = recv(client,data+r,len-r,0);
    if(n==-1) error("recv_all");
    else if(n==0){
      fprintf(stderr, "EOF\n");
      return r;
    }
    r+=n;
  }
  return r;
}

int send_all(int client,char *data,int len){
  int r=0,n=0;
  while(r<len){
    n = send(client,data+r,len-r,0);
    if(n==-1) error("send_all");
    else if(n==0){
      fprintf(stderr, "EOF\n");
      return r;
    }
    r+=n;
  }
  return r;
}
