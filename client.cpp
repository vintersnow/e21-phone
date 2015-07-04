#include "common.h"
#include "connect.h"
#include "pthread.h"

struct Data
{
  char data[N];
  bool not_send;
  int len;
};

struct data_set{
  int conn;
  FILE *fp;
  int index;
  Data data[2];
};



pthread_mutex_t send_mutex;

void* _read(void *args){
  printf("read\n");
  data_set *data = (data_set *)args;
  int index;
  Data *d;
  while(1){
    index = (data->index + 1)%2;
    d = &(data->data[index]);
    d->len=fread(d->data,sizeof(char),N,data->fp);
    if(d->len<0) error("read");
    if(d->len == 0) break;

    d->not_send = true;
  }
  return NULL;
}

void* _send(void *args){
  data_set *data = (data_set *)args;
  int index;
  printf("send\n");
  Data *d;
  while(1){
    index = data->index;
    d = &(data->data[index]);
    if(d->not_send){
      if(send(data->conn,d->data,d->len,0)<0) error("send error");
      d->not_send = false;
    }else{
      pthread_mutex_lock(&send_mutex);
      data->index = (index+1)%2;
      pthread_mutex_unlock(&send_mutex);
    }
  }
  return NULL;
}

void* my_send(void *args){
  // data_set * data = (data_set *)args;
  // char in_data[2][N];
  pthread_mutex_init(&send_mutex, NULL);
  pthread_t th[2];
  pthread_create(&th[0],NULL,_read,args);
  pthread_create(&th[1],NULL,_send,args);
  for (int i = 0; i < 2; ++i)
  {
    pthread_join(th[i],NULL);
  }
  pthread_mutex_destroy(&send_mutex);

  // ssize_t n;
  // while(1){
  //   memset(in_data,'\0',N);
  //   n = fread(in_data,sizeof(char),N,data->fp);
  //   // printf("%ld\n",n);
  //   if(n<0) error("read in_data error");
  //   if(n==0) break;
  //   if((n=send(data->conn,in_data,n,0))<0) error("send error");
  //   // printf("%ld\n", n);
  //   // if(send_all(data->conn,in_data,N)<0) error("send error");
  //   // printf("send\n");
  // }
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

  // const char *cmd = "rec -V1 -q -t raw -b 16 -c 1 -e s -r 44100 -";
  FILE *fp;
  if((fp=popen(COMMAND,"r"))==NULL) error("popen");
  // const char *cmd_play = "play -q -t raw -b 16 -c 1 -e s -r 44100 -";
  FILE *fp_p;
  if((fp_p = popen(COMMAND2,"w"))==NULL) error("popen play");

  // char in_data[N],out_data[N];
  // ssize_t n;
  pthread_t th[2];
  data_set data[2];
  data[0].conn = serv;
  data[1].conn = serv;
  data[0].fp = fp_p;
  data[1].fp = fp;

  data[1].data[0].not_send = false;
  data[1].data[1].not_send = false;
  data[1].index = 0;

  pthread_create(&th[0],NULL,my_recv,&data[0]);
  pthread_create(&th[1],NULL,my_send,&data[1]);

  for (int i = 0; i < 2; ++i)
  {
    pthread_join(th[i],NULL);
  }
  shutdown(serv,SHUT_WR);
  pclose(fp);

  printf("shutdown\n");

  return 0;
}
