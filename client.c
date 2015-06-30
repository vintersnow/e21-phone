#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <fcntl.h>
#include <unistd.h>

#ifndef N
#define N 5000
#endif

void error(const char * str){
  perror(str);
  exit(1);
}

int main(int argc, char const *argv[])
{
  if(argc!=3) error("wrong parameter");
  const char * ip = argv[1];
  int port = atoi(argv[2]);
  printf("ip:%s port:%d\n",ip,port);

  int serv = socket(PF_INET,SOCK_STREAM,0);
  if(serv==-1) error("socket");

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  if(inet_aton(ip,&addr.sin_addr)==0) error("error with ip");
  addr.sin_port = htons(port);
  int ret = connect(serv,(struct sockaddr *)&addr,sizeof(addr));
  if(ret==-1) error("can not connect");
  printf("connecttion success\n");

  const char *cmd = "rec -t raw -b 16 -c 1 -e s -r 44100 -";
  FILE *fp;
  if((fp=popen(cmd,"r"))==NULL) error("popen");
  const char *cmd_play = "play -t raw -b 16 -c 1 -e s -r 44100 -";
  FILE *fp_p;
  if((fp_p = popen(cmd_play,"w"))==NULL) error("popen play");

  char in_data[N],out_data[N];
  ssize_t n;
  while(!feof(fp)){
    memset(out_data,'\0',N);
    n = recv(serv,out_data,N,0);
    if(n<0) error("recv out_data error");
    if(n==0) break;
    // write(1,out_data,n);
    fwrite(out_data,sizeof(char),n,fp_p);

    memset(in_data,'\0',N);
    n = fread(in_data,sizeof(char),N,fp);
    if(n<0) error("read in_data error");
    if(n==0) break;
    if(send(serv,in_data,n,0)<0) error("send error");
  }


  shutdown(serv,SHUT_WR);
  pclose(fp);

  printf("shutdown\n");

  return 0;
}
