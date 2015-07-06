#ifndef DEFF2015_6_29
#define N 512 //for window_func
// #define N 8192
#define COMMAND "rec -q -t raw -b 16 -c 1 -e s -r 44100 -"
#define COMMAND2 "play -q -t raw -b 16 -c 1 -e s -r 44100 -"
// #define N 10000

////for debug
#ifdef __APPLE__
#include "chrono"
#include "iostream"
#endif
#define __DEBUG__


#endif
