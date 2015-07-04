#include "common.h"
#include "connect.h"
#include "pthread.h"
#include "fft.h"


using namespace std;

struct Data
{
  short data[N];
  bool not_send;
  long len;
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
    d->len=fread(d->data,sizeof(short),N,data->fp);
    if(d->len<0) error("read");
    if(d->len == 0) break;
    if(d->not_send)
      printf("already read\n");
    else
      d->not_send = true;

  }
  return NULL;
}

void* _send(void *args){
  data_set *data = (data_set *)args;
  int index;
  printf("send\n");
  Data *d;
  complex<double> *X = new complex<double>[FN];
  complex<double> *Y = new complex<double>[FN];
  const long cut = BOTTOM*FN/R+1;

  while(1){
    index = data->index;
    d = &(data->data[index]);
    if(d->not_send){
      sample_to_complex(d->data,X,FN);
      fft(X,Y,FN);
      if(send(data->conn,Y+cut,BUFFER_SIZE,0)<0) error("send error");
      d->not_send = false;
    }else{
      pthread_mutex_lock(&send_mutex);
      data->index = (index+1)%2;
      pthread_mutex_unlock(&send_mutex);
    }
  }
  free(X);free(Y);
  return NULL;
}

void* my_send(void *args){
  // data_set * data = (data_set *)args;
  // short in_data[N];
  // short out_data[N];
  pthread_mutex_init(&send_mutex, NULL);
  pthread_t th[2];
  pthread_create(&th[0],NULL,_read,args);
  pthread_create(&th[1],NULL,_send,args);
  for (int i = 0; i < 2; ++i)
  {
    pthread_join(th[i],NULL);
  }
  pthread_mutex_destroy(&send_mutex);

  // complex<double> *X = new complex<double>[FN];
  // complex<double> *Y = new complex<double>[FN];
  // const long cut = BOTTOM*FN/R+1;
// int fp;
  // if((fp=open("./test_cl",O_WRONLY,O_CREAT))<0) error("open");
  // ssize_t n;
  // while(1){
  //   memset(in_data,0,sizeof(short) * N);
  //   n = fread(in_data,sizeof(short),N,data->fp);
  //   // printf("%ld\n",n);
  //   if(n<0) error("read in_data error");
  //   if(n==0) break;
  //   sample_to_complex(in_data,X,FN);
  //   fft(X,Y,FN);
  //   if((n=send(data->conn,Y+cut,BUFFER_SIZE,0))<0) error("send error");
  //   // if(send_all(data->conn,in_data,N)<0) error("send error");

  // }
  return NULL;
}

void* my_recv(void*args){
  data_set *data = (data_set *)args;
  short out_data[N];
  ssize_t n;
  complex<double> * X = new complex<double>[FN];
  complex<double> * Y = new complex<double>[FN];
  const long cut = BOTTOM*FN/R+1;
  // const long n_data = (TOP - BOTTOM) * FN / R;

  while(1){
    // memset(out_data,0,sizeof(short) * N);
    memset(Y,0,FN*sizeof(complex<double>));
    // printf("start recv\n");
    // n = recv(data->conn,out_data,N,0);
    n=recv(data->conn,Y+cut,BUFFER_SIZE,0);
    // n = recv_all(data->conn,out_data,N);
    if(n<0) error("recv out_data error");
    if(n==0) break;
    ifft(Y,X,FN);
    complex_to_sample(X,out_data,FN);
    fwrite(out_data,sizeof(short),N,data->fp);
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

  FILE *fp;
  if((fp=popen(COMMAND,"r"))==NULL) error("popen");
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
