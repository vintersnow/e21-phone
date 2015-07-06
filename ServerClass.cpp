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
  // pthread_create(&ft,NULL,Server::lanch_free_thread,this);

  printf("Server start\n");

}

Server::~Server(){
  pthread_cancel(ft);
  pthread_join(ft,NULL);
  pthread_mutex_destroy(&mutex);
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

  // const auto startTime = std::chrono::system_clock::now();
  // for(it = clients.begin(); it!=clients.end(); it++){
 // pthread_mutex_lock(&mutex);

  while(it!=clients.end()){
    c = it->first;
    live = it->second;
    it++;
    if(live == false) free_client(c);
    else if(c->stop == true) free_client(c);
    else if(c != c_o){
      // strcpy(c->sendbuf,buf);
      c->sendbuf = buf;
      c->sendlen = len;

      // c->sender(buf,len);
      pthread_create(&pt[i++],NULL,Client::lanch_sender,c);

    }
  }
  // if(i==0) delete(pt);
  // else th_m[pt] = i;


  for (int j = 0; j < i; ++j)
  {
    pthread_join(pt[j],NULL);
  }
  delete(pt);
  // pthread_mutex_unlock(&mutex);
      // const auto endTime = std::chrono::system_clock::now();
    // const auto timeSpan = endTime - startTime;
    // std::cout << "処理時間:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
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

void Server::free_thread(){
  std::map<pthread_t*,int>::iterator it;
  pthread_t *pt;
  while(1){
    if(th_m.size()>0){
      it = th_m.begin();
      pt = it->first;
      for (int i = 0; i < it->second; ++i)
      {
        pthread_join(pt[i],NULL);
      }
      th_m.erase(it);
      delete(pt);
      // printf("free thread\n");
    }
  }
}
