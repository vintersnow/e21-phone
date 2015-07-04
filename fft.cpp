#include <assert.h>
#include <complex>
#include <math.h>

#include "fft.h"

typedef short sample_t;
FILE *p_file, *p_file2;
char buf[N], buf2[N];

// typedef _Complex complex;

using namespace std;

void server(char **);
void client(char **);
void send_recv(int);
//void marge(char *, short*);
//void unfold(short *, char*);
void sample_to_complex(short *, complex<double>*, long);
void complex_to_sample(complex<double> *, short *, long);
void fft_r(complex<double> *, complex<double> *, long, complex<double>);
void fft(complex<double> *, complex<double> *, long);
void ifft(complex<double> *, complex<double> *, long);
void zero_data(complex<double> *, complex<double> *);
void cut_data(complex<double> *, complex<double> *);

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
  int n_data = (TOP - BOTTOM) * FN / R; //number of datas to send
  complex<double> * send_data = new complex<double>[FN];
  complex<double> * get_data = new complex<double>[FN];
  complex<double> * X = new complex<double>[FN];
  complex<double> * Y = new complex<double>[FN];
  int cut = BOTTOM*FN/R+1;
  while (1) {
    m = fread(in_data, sizeof(short), N, p_file);
    if (m == 0) break;
    sample_to_complex(in_data, X, FN);
    fft(X, Y, FN);
    // cut_data(Y, send_data);
    // printf("%f\n", (double)sizeof(complex<double>)*n_data/N);
    send(s, Y+cut, sizeof(complex<double>) * n_data,0);

    n = recv(s, Y+cut, sizeof(complex<double>) * n_data,0);
    // zero_data(send_data, Y);
    ifft(Y, X, FN);
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

void fft(complex<double> * x, complex<double> * y, long n) {
  long i;
  double arg = 2.0 * M_PI / n;
  // complex<double> w = cos(arg) - 1.0i * sin(arg);
  complex<double>w(cos(arg),-sin(arg));
  fft_r(x, y, n, w);

  for (i = 0; i < n; i++) {
    if ((i*R/n < BOTTOM) || (i*R/n > TOP)) {
      y[i] = 0;  //filter
    }
    else y[i] /= n;
  }
}

void ifft(complex<double> * y, complex<double> * x, long n) {
  double arg = 2.0 * M_PI / n;
  // complex<double> w = cos(arg) + 1.0i * sin(arg);
  complex<double> w(cos(arg),sin(arg));
  fft_r(y, x, n, w);
}

void* fft_r_th(void *args){
  
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

int main(int argc, char **argv) {
  if (argc == 2) server(argv);
  else client(argv);
  return 0;
}
