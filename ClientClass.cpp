#include "ClientServerClass.h"

void Client::set_name(char *name,char *str){
  memset(name,'\0',MAX_NAME_LEN);
  strncpy(name,str,MAX_NAME_LEN-1);
}

// Client::Client(Server *s, char *name,int conn,struct sockaddr_in *client_addr,socklen_t *len){
//   this->s = s;
//   this->conn = conn;
//   if(name==NULL){
//     char ss[MAX_NAME_LEN*10];
//     char num[10];
//     sprintf(num,"%d",conn);
//     strcpy(ss,"U.N.Owen");
//     strcat(ss,num);

//     set_name(this->name,ss);
//   }
//   this->stop = false;
//   this->def_addr = *client_addr;
//   this->def_addr_len = *len;

//   pthread_mutex_init(&mutex, NULL);
//   pthread_create(&th_receiver,NULL,Client::lanch_receiver,this);

//   printf("new Client %s connected\n",this->name);
// }

Client::Client(Server *s,int def_conn,int port,struct sockaddr_in *client_addr,socklen_t *len){
  this->s = s;
  this->stop = false;
  this->def_conn = def_conn;
  this->def_addr = *client_addr;
  this->def_addr_len = *len;

  listener = socket(PF_INET,SOCK_STREAM,0);
  if(listener==-1) error("socket");

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  if(bind(listener,(struct sockaddr *)&addr,sizeof(addr))<0) error("bind error");

  if(listen(listener,5)<0) error("linten error");

  printf("new Client %s connected\n",this->name);
}

void Client::start(char *name,int conn,struct sockaddr_in *client_addr,socklen_t *len){
  this->conn = conn;
  if(name==NULL){
    char ss[MAX_NAME_LEN*10];
    char num[10];
    sprintf(num,"%d",conn);
    strcpy(ss,"U.N.Owen");
    strcat(ss,num);

    set_name(this->name,ss);
  }
   pthread_mutex_init(&mutex, NULL);
   pthread_create(&th_receiver,NULL,Client::lanch_receiver,this);

}

Client::~Client(){
  pthread_cancel(th_receiver);
  // pthread_cancel(th_sender);

  pthread_join(th_receiver,NULL);
  // pthread_join(th_sender,NULL);

  pthread_mutex_destroy(&mutex);

  close(conn);
  close(def_conn);
}

void Client::cl_read(){
  // int n;
  // memset(c->sendbuf,'\0',N);
  // char buf[N];
  // memset(readbuf,'\0',BUFFER_SIZE);
  recv_all(conn,readbuf,BUFFER_SIZE);
}

void Client::cl_send(char *buf ,int len){
  // printf("try send\n");
}
void Client::cl_send(){
  // cl_send(s->sendbuf,s->sendlen);
}

void Client::cl_stop(){
  // free();
  pthread_cancel(th_receiver);
  // pthread_cancel(th_sender);
  stop = true;
  close(conn);
  printf("client %s disconnected\n",name);
}

void Client::sender(char *buf ,int len){
  int n;
  if(len > 0){
    // printf("send:%d\n", def_conn);
    // pthread_mutex_lock(&mutex);

    const auto startTime = std::chrono::system_clock::now();

    if((n=send(conn,buf,len,0))<0) cl_stop();
    
     const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    std::cout << "処理時間:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;

    // pthread_mutex_unlock(&mutex);
  }
}


void Client::receiver(){

  // printf("start receiver\n");
  ssize_t n;
  while(1){
    // printf("read:%d\n", def_conn);
    memset(readbuf,'\0',BUFFER_SIZE);
    // pthread_mutex_lock(&(s->mutex));
    // printf("start recv\n");

    n = recv(conn,readbuf,BUFFER_SIZE,0);
    // n= recv(conn,Y+cut,BUFFER_SIZE,0);


    // pthread_mutex_unlock(&(s->mutex));
    // printf("recv %ld\n",n );
    // write(fp,Y,FN);
    // exit(1);
    // if(n<0)error("receiver error");
    // if(n<0) break;
    if(n<=0) break;
    // const auto startTime = std::chrono::system_clock::now();
    s->broadcast(this,readbuf,n);
    // const auto endTime = std::chrono::system_clock::now();
    // const auto timeSpan = endTime - startTime;
    // std::cout << "処理時間:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;

    // if((n=send(conn,sendbuf,n,0))<0) cl_stop();
  }
  cl_stop();
}



