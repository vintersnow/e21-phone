#include "fft.h"

typedef short sample_t;
FILE *p_file, *p_file2;
char buf[N], buf2[N];
#define Therad_num 3 //2~5ほどがよさそう。
using namespace std;

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

// int main(int argc, char **argv) {
//   if (argc == 2) server(argv);
//   else client(argv);
//   return 0;
// }
