#ifndef BASE_H_INCLUDED
#define BASE_H_INCLUDED

#include "SDL2/SDL.h"
#include "Framework.h"
#include "Program.h"

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

class CrimsonlandFramework : public Framework {

public:

  CrimsonlandFramework(int argc, char** commands);

  virtual void PreInit(int& width, int& height, bool& fullscreen);

  virtual bool Init();

  virtual void Close();

  virtual bool Tick();

  virtual void onMouseMove(int x, int y, int xrelative, int yrelative) { }

  virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) { }

  virtual void onKeyPressed(FRKey k) { }

  virtual void onKeyReleased(FRKey k) { }

private:

  bool initScreenTexture(int screenW, int screenH);
  void activateTextureRendering();
  void deactivateTextureRendering();
  void drawTextureToScreen();

  WorldData m_worldData;

  // TODO(mizofix): move texture to abstract layer
  SDL_Texture* m_screenTexture;
  Sprite* m_testSprite;

  Program m_program;
};

#endif
