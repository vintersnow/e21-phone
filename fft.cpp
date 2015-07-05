#include "fft.h"

typedef short sample_t;
FILE *p_file, *p_file2;
char buf[N], buf2[N];
#define Therad_num 3 //2~5ほどがよさそう。
using namespace std;

void server(char **argv) {
  int portnum = atoi(argv[1]);

  int ss = socket(PF_INET, SOCK_STREAM, 0); //error: return -1
  if (ss == -1) {
    printf("making socket failed/n");
    return;
  }

  struct sockaddr_in addr; /* bind に渡すアドレス情報 */
  addr.sin_family = AF_INET; /* IPv4 */
  addr.sin_port = htons(portnum); /*ポート##で待受 */
  addr.sin_addr.s_addr = INADDR_ANY; /* どのIPアドレスでも待受 */
  if(bind(ss, (struct sockaddr *)&addr, sizeof(addr)) == -1) { //error: return -1
    printf("bind failed\n");
    return;
  }

  if (listen(ss, 10) == -1) {
    printf("listen failed\n");
    return;
  }

  struct sockaddr_in client_addr;
  socklen_t len = sizeof(struct sockaddr_in);
  int s = accept(ss, (struct sockaddr *)&client_addr, &len);
  if (s == -1) {
    printf("accept failed\n");
    return;
  }

  close(ss);

  send_recv(s);

  close(s);

  return;
}

void client (char **argv) {

  int s = socket(PF_INET, SOCK_STREAM, 0); //error: return -1
  if (s == -1) {
    printf("making socket failed/n");
    return;
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET; // def IPv4
  int aton = inet_aton(argv[1] , &addr.sin_addr);
  if (aton == 0) {
    printf("inet_aton\n");
    return;
  }
  int portnum = atoi(argv[2]);
  addr.sin_port = htons(portnum); //port number
  int ret = connect (s, (struct sockaddr *)&addr, sizeof(addr)); //connect; error: return -1
  if (ret == -1) {
    printf("connection failed\n");
    return;
  }

  send_recv(s);

  close(s);
  return;
}

void send_recv(int s) {
  if(!(p_file = popen(COMMAND, "r"))) {
    printf("popen failed\n");
    return;
  }
  if(!(p_file2 = popen(COMMAND2, "w"))) {
    printf("popen failed\n");
    return;
  }

  int m, n;
  short in_data[N], out_data[N];
  // int n_data = (TOP - BOTTOM) * FN / R; //number of datas to send
  // complex<double> * send_data = new complex<double>[FN];
  // complex<double> * get_data = new complex<double>[FN];
  complex<double> * X = new complex<double>[FN];
  complex<double> * Y = new complex<double>[FN];
  complex<double> * Z = new complex<double>[FN];
  int cut = BOTTOM*FN/R+1;
  printf("start send_recv\n");
  while (1) {
    m = fread(in_data, sizeof(short), N, p_file);
    if (m == 0) break;
    sample_to_complex(in_data, X, FN);
    sample_to_complex(in_data,Z,FN);
    multi_winfunc(X);
    // i_winfunc(X);
    // for (int i = 0; i < FN; ++i)
    // {
    //   printf("diff:%f\n",real(X[i])-real(Z[i]));
    // }
    // exit(1);
    // printf("start fft\n");
    fft(X, Y, FN);
    // printf("fin fft\n");
    // cut_data(Y, send_data);
    // printf("%f\n", (double)sizeof(complex<double>)*n_data/N);
    // send(s, Y+cut, BUFFER_SIZE,0);
    send(s, Y,FN*sizeof(complex<double>) ,0);

    // n = recv(s, Y+cut, BUFFER_SIZE,0);
    n = recv(s,Y,FN*sizeof(complex<double>),0);
    // zero_data(send_data, Y);
    ifft(Y, X, FN);
    i_winfunc(X);
    complex_to_sample(X, out_data, FN);
    fwrite(out_data, sizeof(short), N, p_file2);
  }

  pclose(p_file);
  pclose(p_file2);

  return;
}

void cut_data(complex<double> * in_data, complex<double> *out_data) {
  int i;
  for (i = 0; i < FN; i++) {
    if ((i*R/FN < BOTTOM) || (i*R/FN > TOP)) ;
    else out_data[i-1-BOTTOM*FN/R] = in_data[i];
  }
  return;
}

void zero_data(complex<double> * in_data, complex<double> *out_data) {
  int i;
  for (i = 0; i < FN; i++) {
    if ((i*R/FN < BOTTOM) || (i*R/FN > TOP)) out_data[i] = 0;
    else out_data[i] = in_data[i-1-BOTTOM * FN / R];
  }
  return;
}

/*
void marge(char *input, short *output) {
  int i;
  for (i = 0; i < FN; i++) {
    output[i] = input[2*i] * 256 + input[2*i+1];
  }
  return;
}

void unfold(short *input, char *output) {
  int i;
  for (i = 0; i < FN*2; i++) {
    if (i%2 == 1) output[i] = (char)input[i/2];
    else output[i] = input[i/2]>>8;
  }
  return;
}
*/

void sample_to_complex(short * s, complex<double> * X, long n) {
  long i;
  for (i = 0; i < n; i++) X[i] = s[i];
}

void complex_to_sample(complex<double> * X, short * s, long n) {
  long i;
  for (i = 0; i < n; i++) {
    s[i] = real(X[i]);
  }
}

void set_com_struct(complex_set *c,pthread_mutex_t *m, complex<double>*x,complex<double>*y,long n,complex<double>w){
  c->m = m;
  c->x = x;
  c->y = y;
  c->w = w;
  c->n = n;
}

void fft(complex<double> * x, complex<double> * y, long n) {
  long i;
  double arg = 2.0 * M_PI / n;
  // complex<double> w = cos(arg) - 1.0i * sin(arg);
  complex<double>w(cos(arg),-sin(arg));
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_init(&mutex,NULL);
  complex_set c;
  set_com_struct(&c,&mutex,x,y,n,w);
  // fft_r(x, y, n, w);
  fft_r_th((void *)&c);
  pthread_mutex_destroy(&mutex);

  for (i = 0; i < n; i++) {
    // if ((i*R/n < BOTTOM) || (i*R/n > TOP)) {
    //   y[i] = 0;  //filter
    // }
    // else y[i] /= n;
    y[i] /= n;
  }
}

void ifft(complex<double> * y, complex<double> * x, long n) {
  double arg = 2.0 * M_PI / n;
  // complex<double> w = cos(arg) + 1.0i * sin(arg);
  complex<double> w(cos(arg),sin(arg));
   pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_init(&mutex,NULL);
  complex_set c;
  set_com_struct(&c,&mutex,y,x,n,w);
  // fft_r(y, x, n, w);
  fft_r_th((void*)&c);
  pthread_mutex_destroy(&mutex);
}


void* fft_r_th(void *args){
  complex_set *c = (complex_set *)args;
  complex<double> *x = c->x;
  complex<double> *y = c->y;
  complex<double> w = c->w;
  long half_n = c->n / 2;
  pthread_mutex_t *m = c->m;

  pthread_t th[2];
  // printf("%ld\n", c->n);

  if (c->n == 1) { y[0] = x[0]; }
  else {
    // complex<double> W = 1.0;
    complex<double>W(1.0,0);
    long i;
    for (i = 0; i < half_n; i++) {
      y[i]     =     (x[i] + x[i+half_n]); /* 偶数行 */
      y[i+half_n] = W * (x[i] - x[i+half_n]); /* 奇数行 */
      W *= w;
    }
    complex_set c1,c2;
    set_com_struct(&c1,m,y,x,half_n,w*w);
    set_com_struct(&c2,m,y+half_n,x+half_n,half_n,w*w);
    // fft_r(y,     x,     n/2, w * w);
    // fft_r(y+n/2, x+n/2, n/2, w * w);
    if(c->n * pow(2,Therad_num-1) >= N+1){
      // printf("thread create\n");
      pthread_create(&th[0],NULL,fft_r_th,(void*)&c1);
      pthread_create(&th[1],NULL,fft_r_th,(void*)&c2);
      for (int i = 0; i < 2; ++i)
      {
        pthread_join(th[i],NULL);
      }
    }else{
      // printf("not create\n");
      fft_r_th((void*)&c1);
      fft_r_th((void*)&c2);
    }
    for (i = 0; i < half_n; i++) {
      y[2*i]   = x[i];
      y[2*i+1] = x[i+half_n];
    }
  }
  return NULL;
}

void fft_r(complex<double> * x, complex<double> * y, long n, complex<double> w) {
  if (n == 1) { y[0] = x[0]; }
  else {
    // complex<double> W = 1.0;
    complex<double>W(1.0,0);
    long i;
    for (i = 0; i < n/2; i++) {
      y[i]     =     (x[i] + x[i+n/2]); /* 偶数行 */
      y[i+n/2] = W * (x[i] - x[i+n/2]); /* 奇数行 */
      W *= w;
    }
    fft_r(y,     x,     n/2, w * w);
    fft_r(y+n/2, x+n/2, n/2, w * w);
    for (i = 0; i < n/2; i++) {
      y[2*i]   = x[i];
      y[2*i+1] = x[i+n/2];
    }
  }
}

double window_func(int n){
  return 0.5-0.5*cos(2*M_PI*n/N);
}

void multi_winfunc(complex<double> *data){
  for (int i = 0; i < FN; ++i)
  {
    // printf("%f*%f\n", real(data[i]),window_func(i));
    data[i] *= window_func(i);
  }
  // exit(1);
}

void i_winfunc(complex<double> *data){
  for (int i = 0; i < FN; ++i)
  {
    data[i] /= window_func(i);
  }
}

// int main(int argc, char **argv) {
//   if (argc == 2) server(argv);
//   else client(argv);
//   return 0;
// }
