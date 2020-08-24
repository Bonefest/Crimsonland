#ifndef BASE_H_INCLUDED
#define BASE_H_INCLUDED

#include "Framework.h"
#include "Program.h"
#include "Common.h"
#include "Assert.h"

#include "Math.h"
#include "Message.h"

#include "ecs/Registry.h"
#include "ecs/SystemManager.h"

#include "Systems.h"

#include <array>
#include <vector>
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

  virtual void onMouseWheel(int y);

  virtual void onKeyPressed(FRKey k);

  virtual void onKeyReleased(FRKey k) { }

private:

  bool initMainPart();
  bool initECS();
  bool initPlants();

  void clearMainPart();
  void clearPlants();

  bool restartGame();

  void update();
  void draw();
  void drawToScreen();
  void updateTimer();

  void drawBackground();
  void drawPlants(std::vector<std::pair<Sprite*, vec2>>& plants);
  void drawUI();

  void onPlayerDead(Message message);
  void processDeadMessage();

  bool      m_done;

  WorldData m_worldData;

  Registry*          m_registry;
  ECSContext         m_context;
  UIRenderingSystem* m_uiSystem;
  SystemManager      m_systemManager;

  Sprite* m_background;
  std::vector<std::pair<Sprite*, vec2>> m_bushes;
  std::vector<std::pair<Sprite*, vec2>> m_trees;

  Texture* m_screenTexture;

  Program m_bumpProgram;

  bool m_playerDeadMessageReceived;
  bool m_playerDeadMessageProcessed;

  int  m_deadCameraPosX;
  int  m_deadCameraPosY;

  float m_lastTime;
  float m_deltaTime;

};


#endif
