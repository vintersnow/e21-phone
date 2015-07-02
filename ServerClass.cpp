#include "ClientServerClass.h"

Server::Server(int port){
  listener = socket(PF_INET,SOCK_STREAM,0);
  if(listener==-1) error("socket");

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  if(bind(listener,(struct sockaddr *)&addr,sizeof(addr))<0) error("bind error");

  if(listen(listener,10)<0) error("linten error");

  pthread_mutex_init(&mutex, NULL);
  const char *cmd_play = "play -q -t raw -b 16 -c 1 -e s -r 44100 -";
  if((fp = popen(cmd_play,"w"))==NULL) error("popen play");

}

Server::~Server(){
  close(listener);
}

void Server::broadcast(Client *c_o,char *buf,int len){
  // printf("broadcast!!\n");
  std::map<Client*,bool>::iterator it;
  Client *c;
  int s = clients.size();
  // printf("%d\n",s );
  // pthread_t *pt = new pthread_t[s];
  // int i=0;
  for(it = clients.begin(); it!=clients.end(); it++){
    c = it->first;
    if(it->second == false) c->cl_stop();
    else if(c != c_o){
      // printf("send yes ok\n");
      // strncpy(c->sendbuf,buf,N);

      // pthread_mutex_lock(&mutex);
      // strcpy(c->sendbuf,buf);
      // c->sendbuf = buf;
      // pthread_mutex_unlock(&mutex);

      c->cl_send(buf,len);
      // pthread_create(&pt[i++],NULL,Client::lanch_send,c);
      // printf("really??\n");
    }
  }

  // fwrite(buf,sizeof(char),N,fp);
  // c_o->sendbuf = buf;
  // c_o->cl_send(buf,len);
  // for (int j = 0; j < s; ++j)
  // {
  //   pthread_join(pt[j],NULL);
  // }
  // delete(pt);
  // printf("end broadcast\n");
}
