#ifndef FFT_2015_0704
#define FFT_2015_0704

#include "common.h"
#include "connect.h"

#define FN N
#define R 44100
#define COMMAND "rec -t raw -b 16 -c 1 -e s -r 44100 -"
#define COMMAND2 "play -t raw -b 16 -c 1 -e s -r 44100 -"
#define BOTTOM 100 // cut-off freq. bottom
#define TOP 4000   // cut-off freq. top

#endif
