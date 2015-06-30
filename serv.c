#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <fcntl.h>
#include <unistd.h>

#ifndef N
#define N 10000
#endif

void error(const char * str){
  perror(str);
  exit(1);
}

int main(int argc, char const *argv[])
{

  if(argc!=2) error("wrong parameter");
  int port = atoi(argv[1]);
  printf("port:%d\n",port);

  int listener = socket(PF_INET,SOCK_STREAM,0);
  if(listener==-1) error("socket");

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  if(bind(listener,(struct sockaddr *)&addr,sizeof(addr))<0) error("bind error");

  if(listen(listener,10)<0) error("linten error");

  struct sockaddr_in client_addr;
  socklen_t addr_len = sizeof(struct sockaddr_in);
  int client = accept(listener,(struct sockaddr *)&client_addr,&addr_len);
  if(client<0) error("client error");

  const char *cmd = "rec -t raw -b 16 -c 1 -e s -r 44100 -";
  FILE *fp;
  if((fp=popen(cmd,"r"))==NULL) error("popen");
  const char *cmd_play = "play -t raw -b 16 -c 1 -e s -r 44100 -";
  FILE *fp_p;
  if((fp_p = popen(cmd_play,"w"))==NULL) error("popen play");

  char in_data[N],out_data[N];
  ssize_t n;
  while(!feof(fp)){
    // memset(in_data,'\0',N);
    n = fread(in_data,sizeof(char),N,fp);
    if(n<0) error("read in_data error");
    if(n==0) break;
    if(send(client,in_data,n,0)<0) error("send error");

    // memset(out_data,'\0',N);
    n = recv(client,out_data,N,0);
    if(n<0) error("recv out_data error");
    if(n==0) break;
    // write(1,out_data,n);
    fwrite(out_data,sizeof(char),n,fp_p);
  }

  close(client);
  close(listener);
  pclose(fp);

  return 0;
}
