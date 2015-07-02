#ifndef ClientServerClass20150702

#include "common.h"
#include "connect.h"
#include "pthread.h"
// #include "ClientServerClass.h"

#define ClientServerClass20150702
#define MAX_NAME_LEN 20

class Server;
class Client;

class Client{
private:
  pthread_t th_receiver;
  pthread_t th_sender;
  pthread_mutex_t mutex;

  static void* lanch_receiver(void *p){
    // printf("lanch receiver\n");
    static_cast<Client*>(p)->receiver();
    // printf("is go(r)?\n");
    pthread_exit(NULL);
  };
  static void* lanch_sender(void *p){
    // printf("lanch sender\n");
    static_cast<Client*>(p)->sender();
    // printf("is go(s)?\n");
    pthread_exit(NULL);
  };


public:
  char name[MAX_NAME_LEN];
  int conn;
  bool stop;
  // char sendbuf[N];
  char *sendbuf;
  char readbuf[N];
  struct sockaddr_in addr;
  socklen_t addr_len;
  Server *s;

  static void* lanch_send(void *p){
    static_cast<Client*>(p)->cl_send();
    pthread_exit(NULL);
  }

  Client(Server *s,char *name,int conn,struct sockaddr_in *client_addr,socklen_t *len);
  // Client(){
  //   printf("nothing\n");
  // };
  Client();
  ~Client();
  void cl_send();
  void cl_read();
  void cl_stop();
  void set_name(char *,char *);
  void sender();
  void receiver();
  void run();
};

class Server
{
   struct sockaddr_in addr;
public:
   pthread_mutex_t mutex;
  std::map<Client*, bool> clients;
  void broadcast(Client *c,char *buf);
  int listener;

  Server(int port);
  ~Server();

};
#endif
