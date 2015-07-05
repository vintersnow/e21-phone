#include "fft.h"


int main(int argc, char **argv)
{
  if (argc == 2) server(argv);
  else client(argv);
///////////////////test
 //  short in_data[N];
 //  for (int i = 0; i < N; ++i)
 //  {
 //    in_data[i] = i;
 //  }
 // complex<double> * X = new complex<double>[FN];
 //  complex<double> * Y = new complex<double>[FN]; 
 //  sample_to_complex(in_data, X, FN);
 //  const auto startTime = std::chrono::system_clock::now();
 //    fft(X, Y, FN);
 //    const auto endTime = std::chrono::system_clock::now();
 //    const auto timeSpan = endTime - startTime;
 //    std::cout << "処理時間:" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
 //    exit(1);
  return 0;
}
