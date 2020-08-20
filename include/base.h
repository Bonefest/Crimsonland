#ifndef BASE_H_INCLUDED
#define BASE_H_INCLUDED

#include "SDL2/SDL.h"
#include "Framework.h"
#include "Program.h"
#include "Common.h"
#include "Assert.h"

#include "ecs/Registry.h"
#include "ecs/System.h"
#include "ecs/SystemManager.h"

#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <cstdio>
#include <vector>

#include "Math.h"
#include "Message.h"

#include <array>
#include <unordered_map>

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

  void test(Message message) { }

private:

  void initECS();

  void update();
  void draw();
  void drawToScreen();
  void updateTimer();

  void collisionSystem();
  void penetrationResolution();

  void createZombie();

  WorldData m_worldData;

  Registry  m_registry;
  ECSContext m_context;
  SystemManager m_systemManager;

  std::vector<Entity*> m_entities;
  std::vector<Zombie*> m_zombies;

  Texture* m_screenTexture;

  Program m_bumpProgram;

  float m_lastTime;
  float m_deltaTime;
};


#endif
