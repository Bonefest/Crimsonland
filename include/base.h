#ifndef BASE_H_INCLUDED
#define BASE_H_INCLUDED

#include "Framework.h"
#include <stdlib.h>
#include <math.h>

#include <stdint.h>
#include <cstdio>

#ifndef NDEBUG
#define info(...) fprintf(stderr, __VA_ARGS__)
#else
#define info(...)
#endif

using real = float;

struct WorldData {

  int  numEnemies;
  int  numAmmo;

  int  windowWidth;
  int  windowHeight;

  real mapWidth;
  real mapHeight;


};


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

/* Test Framework realization */
class CrimsonlandFramework : public Framework {

public:

  CrimsonlandFramework(int argc, char** commands);

  virtual void PreInit(int& width, int& height, bool& fullscreen) {
    width = 640;
    height = 480;
  }

  virtual bool Init() { return true;}

  virtual void Close() { }

  virtual bool Tick() { }

  virtual void onMouseMove(int x, int y, int xrelative, int yrelative) { }

  virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) { }

  virtual void onKeyPressed(FRKey k) { }

  virtual void onKeyReleased(FRKey k) { }

private:

  WorldData m_worldData;

};

#endif
