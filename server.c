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

  Client *c;
  while(1){
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    int conn = accept(s->listener,(struct sockaddr *)&client_addr,&addr_len);

    if(conn<0) error("client");
    else{
      c = new Client(s, NULL, conn, &client_addr, &addr_len);
      s->clients[c] = true;
      // printf("connect client %s\n",c->name);
      printf("connect client\n");


    }
  }

  s->free_clients();
  delete(s);

  return 0;
}
