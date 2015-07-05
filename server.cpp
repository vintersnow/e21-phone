#include "ClientServerClass.h"

#define PORT_GAP 10

void* run(void *args){
  return NULL;
}

int main(int argc, char const *argv[])
{

  if(argc!=2) error("wrong parameter");
  int port = atoi(argv[1]);
  printf("port:%d\n",port);

  Server *s=new Server(port);

  Client *c;
  int i=0;
  int cl_port = port + PORT_GAP;
  char tmp[N];
  while(1){
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    int def_conn = accept(s->listener,(struct sockaddr *)&client_addr,&addr_len);
    if(def_conn<0){
      printf("error can't connect to client \n");
      continue;
    }
    else{
      // send(def_conn,&cl_port,sizeof(int),0);
      c = new Client(s, NULL, def_conn, &client_addr, &addr_len);
      s->clients[c] = true;
      i++;
      // printf("connect client %s\n",c->name);
      // printf("connect client\n");
      // printf("%ld\n", s->clients.size());

    }
    if(i>3) break;
  }

  s->free_clients();
  delete(s);

  return 0;
}
