#ifndef DEFF2015_6_29
// #define N 4096 //for window_func
#define N 512
// #define N 8192
#define COMMAND "rec -q -t raw -b 16 -c 1 -e s -r 44100 -"
#define COMMAND2 "play -q -t raw -b 16 -c 1 -e s -r 44100 -"
// #define SEND_BUFFER BUFFER_SIZE
// #define SEND_BUFFER N*sizeof(short)

// #define N 10000

////for debug
#include "chrono"
#include "iostream"
#define __DEBUG__


#endif
