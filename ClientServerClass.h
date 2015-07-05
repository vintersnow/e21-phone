#ifndef ClientServerClass20150702

#include "common.h"
#include "connect.h"
#include "pthread.h"
#include "fft.h"
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
    static_cast<Client*>(p)->receiver();
    pthread_exit(NULL);
  };


public:
  char name[MAX_NAME_LEN];
  int conn;
  bool stop;
  // char sendbuf[N];
  char *sendbuf;
  int sendlen;
  char readbuf[BUFFER_SIZE];
  struct sockaddr_in addr;
  socklen_t addr_len;
  Server *s;


  static void* lanch_sender(void *p){
    Client *c = (Client*)(p);
    c->sender(c->sendbuf,c->sendlen);
    pthread_exit(NULL);
  }

  Client(Server *s,char *name,int conn,struct sockaddr_in *client_addr,socklen_t *len);
  Client();
  ~Client();
  void cl_send(char *buf ,int len);
  void cl_send();
  void cl_read();
  void cl_stop();
  void set_name(char *,char *);
  void sender(char *buf ,int len);
  void receiver();
  void run();
};

class Server
{
private:
   struct sockaddr_in addr;
   static void* lanch_free_thread(void *args){
    ((Server *)args)->free_thread();
    pthread_exit(NULL);
   };
  pthread_t ft;
   void free_thread();
   std::map<pthread_t*,int> th_m;
public:
   pthread_mutex_t mutex;
  std::map<Client*, bool> clients;
  int listener;
  char *sendbuf;
  int sendlen;


  Server(int port);
  ~Server();
  void broadcast(Client *c,char *buf,int len);
  void free_clients();
  void free_client(Client *);

};
#endif
