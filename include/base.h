#ifndef BASE_H_INCLUDED
#define BASE_H_INCLUDED

#include "SDL2/SDL.h"
#include "Framework.h"
#include "Program.h"
#include "Common.h"
#include "Assert.h"

#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <cstdio>

class Entity {
public:

private:
  Sprite* m_sprite;
  float m_posX, m_posY;
  float m_size;
  float fov;

};


class CrimsonlandFramework : public Framework {

public:

  CrimsonlandFramework(int argc, char** commands);

  virtual void PreInit(int& width, int& height, bool& fullscreen);

  virtual bool Init();

  virtual void Close();

  virtual bool Tick();

  virtual void onMouseMove(int x, int y, int xrelative, int yrelative);

  virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) { }

  virtual void onKeyPressed(FRKey k);

  virtual void onKeyReleased(FRKey k) { }

private:

  void update();
  void draw();
  void drawToScreen();
  void updateTimer();

  WorldData m_worldData;

  Texture* m_screenTexture;
  Sprite* m_testSprite;

  float m_spritePosX;
  float m_spritePosY;

  Program m_bumpProgram;

  float m_lastTime;
  float m_deltaTime;

  float m_angle;
};


#endif
