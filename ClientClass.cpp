#include "ClientServerClass.h"

void Client::set_name(char *name,char *str){
  memset(name,'\0',MAX_NAME_LEN);
  strncpy(name,str,MAX_NAME_LEN-1);
}

Client::Client(Server *s, char *name,int conn,struct sockaddr_in *client_addr,socklen_t *len){
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
  // pthread_create(&th_sender,NULL,Client::lanch_sender,this);

}


Client::~Client(){
  pthread_cancel(th_receiver);
  // pthread_cancel(th_sender);

  pthread_join(th_receiver,NULL);
  // pthread_join(th_sender,NULL);

  pthread_mutex_destroy(&mutex);

  close(conn);
}

void Client::cl_read(){
  // int n;
  // memset(c->sendbuf,'\0',N);
  // char buf[N];
  memset(readbuf,'\0',N);
  recv_all(conn,readbuf,N);
}

void Client::cl_send(char *buf ,int len){
  if(len > 0){
    // pthread_mutex_lock(&mutex);
    if(send(conn,buf,len,0)<0) cl_stop();
    // pthread_mutex_unlock(&mutex);
  }
}
void Client::cl_send(){
  cl_send(s->sendbuf,s->sendlen);
}

void Client::cl_stop(){
  // free();
  pthread_cancel(th_receiver);
  // pthread_cancel(th_sender);
  stop = true;
  close(conn);
}

void Client::sender(){
}


void Client::receiver(){

  // printf("start receiver\n");
  ssize_t n;
  while(1){
    memset(readbuf,'\0',N);
    // pthread_mutex_lock(&(s->mutex));
    n = recv(conn,readbuf,N,0);
    // pthread_mutex_unlock(&(s->mutex));

    if(n<0)error("receiver error");
    if(n==0) break;
    s->broadcast(this,readbuf,n);

    // if((n=send(conn,sendbuf,n,0))<0) cl_stop();
  }
}



