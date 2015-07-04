#include "fft.h"

int main(int argc, char **argv)
{
  if (argc == 2) server(argv);
  else client(argv);
  return 0;
}
