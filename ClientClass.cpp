#include "ClientServerClass.h"

void Client::set_name(char *name,char *str){
  memset(name,'\0',MAX_NAME_LEN);
  strncpy(name,str,MAX_NAME_LEN-1);
}

Client::Client(Server *s, char *name,int conn,struct sockaddr_in *client_addr,socklen_t *len){
  // Client *c = new Client();
  // printf("Constructor\n");
  this->s = s;
  this->conn = conn;
  if(name==NULL){
    char ss[MAX_NAME_LEN*10];
    char num[10];
    sprintf(num,"%d",conn);
    strcpy(ss,"U.N.Owen");
    strcat(ss,num);

    set_name(this->name,ss);
  }
  this->stop = false;
  this->addr = *client_addr;
  this->addr_len = *len;

  pthread_mutex_init(&mutex, NULL);
  pthread_create(&th_receiver,NULL,Client::lanch_receiver,this);
  pthread_create(&th_sender,NULL,Client::lanch_sender,this);

  // return this;
  // printf("end Constructor\n");
}

Client::Client(){
  printf("nothing!!!!\n");

}

Client::~Client(){
  pthread_cancel(th_receiver);
  pthread_cancel(th_sender);

  pthread_join(th_receiver,NULL);
  pthread_join(th_sender,NULL);

  pthread_mutex_destroy(&mutex);
}

void Client::cl_read(){
  // int n;
  // memset(c->sendbuf,'\0',N);
  // char buf[N];
  memset(readbuf,'\0',N);
  recv_all(conn,readbuf,N);
}

void Client::cl_send(){
  int len = strlen(sendbuf);
  if(len > 0){
    // pthread_mutex_lock(&mutex);
    int n;
    if((n=send(conn,sendbuf,len,0))<0) cl_stop();
    // printf("%d\n",n);
    // pthread_mutex_unlock(&mutex);
  }
}

void Client::cl_stop(){
  // free();
  pthread_cancel(th_receiver);
  pthread_cancel(th_sender);
  close(conn);
}

void Client::sender(){
  // printf("startSender\n");
  // while(1){

  // }
}


void Client::receiver(){
////////////////////test
  const char *cmd_play = "play -q -t raw -b 16 -c 1 -e s -r 44100 -";
  FILE *fp;
  // if((fp = popen(cmd_play,"w"))==NULL) error("popen play");
  // char out_data[N];
  // ssize_t n;

//////////////////test


  printf("start receiver\n");
  ssize_t n;
  while(1){
    memset(readbuf,'\0',N);
    // printf("start receivering\n");
    // pthread_mutex_lock(&(s->mutex));
    n = recv(conn,readbuf,N,0);
    // pthread_mutex_unlock(&(s->mutex));

    if(n<0)error("receiver error");
    if(n==0) break;
    // printf("receive!!\n");
    // s->broadcast(this,readbuf);
    // fwrite(readbuf,sizeof(char),n,fp);
    // strcpy(sendbuf,readbuf);
    sendbuf = readbuf;
    if((n=send(conn,sendbuf,n,0))<0) cl_stop();
  }
}



