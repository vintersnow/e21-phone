#include "ClientServerClass.h"

Server::Server(int port){
  listener = socket(PF_INET,SOCK_STREAM,0);
  if(listener==-1) error("socket");

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  if(bind(listener,(struct sockaddr *)&addr,sizeof(addr))<0) error("bind error");

  if(listen(listener,5)<0) error("linten error");

  pthread_mutex_init(&mutex, NULL);

  printf("Server start\n");

}

Server::~Server(){
  close(listener);
  printf("Server shutdown\n");
}

void Server::broadcast(Client *c_o,char *buf,int len){
  // printf("broadcast!!\n");
  std::map<Client*,bool>::iterator it= clients.begin();
  Client *c;
  int s = clients.size();
  pthread_t *pt = new pthread_t[s];
  int i=0;
  bool live;

  // for(it = clients.begin(); it!=clients.end(); it++){
  while(it!=clients.end()){
    c = it->first;
    live = it->second;
    it++;
    if(live == false) free_client(c);
    else if(c->stop == true) free_client(c);
    else if(c != c_o){
      // pthread_mutex_lock(&mutex);
      // strcpy(c->sendbuf,buf);
      // pthread_mutex_unlock(&mutex);

      c->sendbuf = buf;
      c->sendlen = len;

      // c->cl_send(buf,len);
      pthread_create(&pt[i++],NULL,Client::lanch_sender,c);
    }
  }

  for (int j = 0; j < i; ++j)
  {
    pthread_join(pt[j],NULL);
  }
  delete(pt);
  // printf("end broadcast\n");
}


void Server::free_clients(){
  std::map<Client*,bool>::iterator it = clients.begin();
  Client *c;
  while(it!=clients.end()){
    c = it->first;
    it++;
    free_client(c);
  }
}

inline void Server::free_client(Client *c){
  printf("free client %s\n", c->name);
  clients.erase(c);
  c->cl_stop();
  delete(c);
}
