#include "ClientServerClass.h"

void* run(void *args){
  return NULL;
}

int main(int argc, char const *argv[])
{

  if(argc!=2) error("wrong parameter");
  int port = atoi(argv[1]);
  printf("port:%d\n",port);

  Server *s=new Server(port);
  std::vector<pthread_t *> pth;

  Client *c;
  while(1){
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    int conn = accept(s->listener,(struct sockaddr *)&client_addr,&addr_len);

// pthread_t *pt;

    if(conn<0) error("client");
    else{
      c = new Client(s, NULL, conn, &client_addr, &addr_len);
      // c = new Client;
      s->clients[c] = true;
      // printf("connect client %s\n",c->name);
      printf("connect client\n");

      // pthread_t *pt = new pthread_t;
      // pth.push_back(new pthread_t);
      // pthread_create(pth.back(),NULL,run,(void *)c);

    }
  }
  std::map<Client*,bool>::iterator it;
  for(it = s->clients.begin(); it!=s->clients.end(); it++){
    Client *c = it->first;
    s->clients.erase(it);
    c->cl_stop();
    delete(c);
  }

  // const char *cmd = "rec -t raw -b 16 -c 1 -e s -r 44100 -";
  // FILE *fp;
  // if((fp=popen(cmd,"r"))==NULL) error("popen");
  // const char *cmd_play = "play -t raw -b 16 -c 1 -e s -r 44100 -";
  // FILE *fp_p;
  // if((fp_p = popen(cmd_play,"w"))==NULL) error("popen play");

  // char in_data[N],out_data[N];
  // ssize_t n;
  // while(!feof(fp)){
  //   // memset(in_data,'\0',N);
  //   n = fread(in_data,sizeof(char),N,fp);
  //   if(n<0) error("read in_data error");
  //   if(n==0) break;
  //   if(send(client,in_data,n,0)<0) error("send error");

  //   // memset(out_data,'\0',N);
  //   // n = recv(client,out_data,N,0);
  //   n = recv_all(client,out_data,N);
  //   if(n<0) error("recv out_data error");
  //   if(n==0) break;
  //   // write(1,out_data,n);
  //   fwrite(out_data,sizeof(char),n,fp_p);
  // }

  // close(client)er
  // close(listener);
  delete(s);
  // pclose(fp);

  return 0;
}
