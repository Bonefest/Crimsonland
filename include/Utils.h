#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "Common.h"


inline int clamp(int value, int min, int max) {
  if(value < min) {
    return min;
  }
  else if(value > max) {
    return max;
  }

  return value;
}


WorldData parseCommands(int argc, char** commands);


#endif
