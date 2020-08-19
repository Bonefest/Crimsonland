#ifndef BASE_H_INCLUDED
#define BASE_H_INCLUDED

#include "SDL2/SDL.h"
#include "Framework.h"
#include "Program.h"
#include "Assert.h"
#include "Texture.h"

#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <cstdio>


class CrimsonlandFramework : public Framework {

public:

  CrimsonlandFramework(int argc, char** commands);

  virtual void PreInit(int& width, int& height, bool& fullscreen);

  virtual bool Init();

  virtual void Close();

  virtual bool Tick();

  virtual void onMouseMove(int x, int y, int xrelative, int yrelative) { }

  virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) { }

  virtual void onKeyPressed(FRKey k);

  virtual void onKeyReleased(FRKey k) { }

private:

  void activateTextureRendering();
  void deactivateTextureRendering();
  void drawTextureToScreen();

  void generateNoise();

  WorldData m_worldData;

  Texture m_screenTexture;
  Texture m_noiseTexture;
  Sprite* m_testSprite;

  float m_spritePosX;
  float m_spritePosY;

  Program m_bumpProgram;
  Program m_noiseProgram;

  float m_lastTime;
  float m_deltaTime;
};

#endif
