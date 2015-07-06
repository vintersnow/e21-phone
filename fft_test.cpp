#include "fft.h"


int main(int argc, char **argv)
{
  if (argc == 2) server(argv);
  else client(argv);
///////////////////test
  short in_data[N];
  for (int i = 0; i < N; ++i)
  {
    in_data[i] = i;
  }
 complex<double> * X = new complex<double>[FN];
  complex<double> * Y = new complex<double>[FN];
  sample_to_complex(in_data, X, FN);
  sample_to_complex(in_data,Y,FN);
  const auto startTime = std::chrono::system_clock::now();
 //    fft(X, Y, FN);
     // int fp = open("./winfunc_in",O_WRONLY);
    // write(fp,in_data,N*sizeof(short));
    // close(fp);
  for (int i = 0; i < 1000; ++i)
  {
    /* code */
    multi_winfunc(X);
    // // printf("%d\n", in_data[N/2]);
    // fp = open("/winfunc_mul",O_WRONLY);

    // write(fp,in_data,N*sizeof(short));
    // exit(1);
    i_winfunc(X);
  }
   const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    std::cout << "処理時間:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;

    for (int i = 0; i < N; ++i)
    {
      printf("X:%f->Y:%f\n",real(X[i]),real(Y[i]));
    }
    // fp = open("/winfunc_i",O_WRONLY);
    // write(fp,in_data,N*sizeof(short));
    // exit(1);
  return 0;
}
