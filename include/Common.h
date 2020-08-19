#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#ifndef NDEBUG
#define info(...) fprintf(stderr, __VA_ARGS__)
#else
#define info(...)
#endif

#include "Math.h"

struct WorldData {

  int  numEnemies;
  int  numAmmo;

  int  windowWidth;
  int  windowHeight;

  real mapWidth;
  real mapHeight;


};

#endif
