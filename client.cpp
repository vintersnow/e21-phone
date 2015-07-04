#include "common.h"
#include "connect.h"
#include "pthread.h"

struct data_set{
  int conn;
  FILE *fp;
};

void* my_send(void*args){
  data_set * data = (data_set *)args;
  char in_data[N];
  ssize_t n;
  while(1){
    memset(in_data,'\0',N);
    n = fread(in_data,sizeof(char),N,data->fp);
    // printf("%ld\n",n);
    if(n<0) error("read in_data error");
    if(n==0) break;
    if((n=send(data->conn,in_data,n,0))<0) error("send error");
    // printf("%ld\n", n);
    // if(send_all(data->conn,in_data,N)<0) error("send error");
    // printf("send\n");
  }
  return NULL;
}

void* my_recv(void*args){
  data_set *data = (data_set *)args;
  char out_data[N];
  ssize_t n;
  while(1){
    memset(out_data,'\0',N);
    // printf("start recv\n");
    n = recv(data->conn,out_data,N,0);
    // n = recv_all(data->conn,out_data,N);
    if(n<0) error("recv out_data error");
    if(n==0) break;
    // printf("%ld\n",n);
    // write(1,out_data,n);
    fwrite(out_data,sizeof(char),n,data->fp);
  }
  return NULL;
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

  const char *cmd = "rec -V1 -q -t raw -b 16 -c 1 -e s -r 44100 -";
  FILE *fp;
  if((fp=popen(cmd,"r"))==NULL) error("popen");
  const char *cmd_play = "play -t raw -b 16 -c 1 -e s -r 44100 -";
  FILE *fp_p;
  if((fp_p = popen(cmd_play,"w"))==NULL) error("popen play");

  // char in_data[N],out_data[N];
  // ssize_t n;
  pthread_t th[2];
  data_set data[2];
  data[0].conn = serv;
  data[1].conn = serv;
  data[0].fp = fp_p;
  data[1].fp = fp;

  // while(!feof(fp)){
    // memset(in_data,'\0',N);
    // n = fread(in_data,sizeof(char),N,fp);
    // if(n<0) error("read in_data error");
    // if(n==0) break;
    // if(send(serv,in_data,n,0)<0) error("send error");
    // if(send_all(serv,in_data,N)<0) error("send error");

    // memset(out_data,'\0',N);
    // n = recv(serv,out_data,N,0);
    // n = recv_all(serv,out_data,N);
    // if(n<0) error("recv out_data error");
    // if(n==0) break;
    // write(1,out_data,n);
    // fwrite(out_data,sizeof(char),n,fp_p);
  // }
  // printf("aaaaaaaaaaaaaaaaaaaaaaaaa\n");
  pthread_create(&th[0],NULL,my_recv,&data[0]);
  pthread_create(&th[1],NULL,my_send,&data[1]);
  // printf("testaksfjlaksl\n");

  for (int i = 0; i < 2; ++i)
  {
    pthread_join(th[i],NULL);
  }
  shutdown(serv,SHUT_WR);
  pclose(fp);

  printf("shutdown\n");

  return 0;
}
