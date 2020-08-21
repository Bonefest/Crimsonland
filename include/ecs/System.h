#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include "Common.h"
#include "Message.h"
#include "Assert.h"
#include <list>

class Registry;

struct ECSContext {
  Registry* registry;
  WorldData data;
};

#include "ecs/Registry.h"

class System {
public:
  virtual ~System() { }

  virtual void init(ECSContext& context) { }
  virtual void update(ECSContext& context, real deltaTime) { }
  virtual void draw(ECSContext& context) { }
};


class PlayerSystem: public System {
public:
  virtual void init(ECSContext& context);
  virtual void update(ECSContext& context, real deltaTime);
  virtual void draw(ECSContext& context);

  void onWeaponPickup(Message message);
  void onPowerupPickup(Message message);
  void onMouseWheel(Message message);

private:
  void generateFootprint(vec2 position, real angle);
  real getPlayerViewDirection();
  Entity getPlayer(ECSContext& context);

  Bitfield m_playerBitfield;
  real m_lastFootprintElapsedTime;

  int m_lastFrameMouseWheel;
};

class ZombieRenderingSystem: public System {
public:
  virtual void draw(ECSContext& context);
};

class PhysicsIntegrationSystem: public System {
public:
  virtual void update(ECSContext& context, real deltaTime);
};


struct Effect {

  Sprite* sprite;
  vec2 position;
  real scale;
  real lifetime;
  real elapsedTime;
  real angle;
  bool fadeOut;

};

using EffectsContainer = std::list<Effect>;

class EffectsSystem: public System {
public:

  virtual void init(ECSContext& context);
  virtual void update(ECSContext& context, real deltaTime);
  virtual void draw(ECSContext& context);
  void onSpawnEffect(Message message);

private:
  EffectsContainer m_effects;
  uint32_t         m_maximalEffectsNumber;
};

#endif
