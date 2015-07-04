#ifndef FFT_2015_0704
#define FFT_2015_0704

#include "common.h"
#include "connect.h"
#include <assert.h>

#define FN N
#define R 44100
#define BOTTOM 100 // cut-off freq. bottom
#define TOP 4000   // cut-off freq. top
#define BUFFER_SIZE (TOP - BOTTOM) * FN / R * sizeof(std::complex<double>)

using namespace std;

struct complex_set
{
  complex<double> *x;
  complex<double> *y;
  complex<double> w;
  long n;
};

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
void server(char **argv);
void client(char **argv);
void* fft_r_th(void*);

#endif
