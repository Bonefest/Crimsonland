#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include "Message.h"
#include "Common.h"
#include "Assert.h"
#include "json.hpp"

#include <list>

class Registry;

struct ECSContext {
  Registry* registry;
  WorldData data;
};

#include "ecs/Registry.h"
#include "ecs/Components.h"

class System {
public:
  virtual ~System() { }

  virtual void init(ECSContext& context) { }
  virtual void update(ECSContext& context, real deltaTime) { }
  virtual void draw(ECSContext& context) { }
};

// NOTE(mizofix): current trail system is frame rate dependent
// to prevent some bugs we should consider to lock frame rate
// or find a better way to draw trail (e.g via OpenGL)
class TrailSystem: public System {
public:

  virtual void update(ECSContext& context, real deltaTime);
  virtual void draw(ECSContext& context);
private:
  TrailParticle generateParticle(const vec2& targetVelocity,
                                 const vec2& position,
                                 real maxAngle,
                                 real maxSpeed);
};

class Player;

class PlayerSystem: public System {
public:
  virtual void init(ECSContext& context);
  virtual void update(ECSContext& context, real deltaTime);
  virtual void draw(ECSContext& context);

  void onWeaponPickup(Message message);
  void onPowerupPickup(Message message);
  void onMouseWheel(Message message);

private:
  void initWeapons(Player* player);
  WeaponData parseWeapon(nlohmann::json& parser);
  void checkCurrentWeapon(Player* player);

  real getPlayerViewDirection();
  Entity getPlayer(ECSContext& context);

  Bitfield m_playerBitfield;
  real m_lastFootprintElapsedTime;

  int m_lastFrameMouseWheel;
};

// class LevelSystem {

// }

class ZombieSystem: public System {
public:
  virtual void init(ECSContext& context);
  virtual void update(ECSContext& context, real deltaTime);
  //virtual void draw(ECSContext& context);


  void onGenerateZombie(Message message);
private:
  Bitfield m_zombieComponents;
};

class ModelRenderingSystem: public System {
public:
  virtual void draw(ECSContext& context);
};

class PhysicsIntegrationSystem: public System {
public:
  virtual void update(ECSContext& context, real deltaTime);
};


// TODO(mizofix): At least Spatial Partioning optimization (or BSP)
class PhysicsCollisionSystem: public System {
public:
  virtual void update(ECSContext& context, real deltaTime);
};

// NOTE(mizofix): Shouldn't we integrate penetration resolution to PhysicsCollisionSystem

using MessageContainer = std::list<Message>;

class PenetrationResolutionSystem: public System {
public:
  virtual void init(ECSContext& context);
  virtual void update(ECSContext& context, real deltaTime);

  void onCollision(Message message);
private:
  MessageContainer m_unprocessedCollisions;
};

class BulletSystem: public System {
public:
  virtual void init(ECSContext& context);
  virtual void update(ECSContext& context, real deltaTime);

  void onCollision(Message message);
private:
  MessageContainer m_unprocessedCollisions;
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

class UIRenderingSystem: public System {
public:

  ~UIRenderingSystem();

  virtual void init(ECSContext& context);
  virtual void update(ECSContext& context, real deltaTime);
  virtual void draw(ECSContext& context);

private:
  Sprite*    m_radarSprite;
  Sprite*    m_weaponSprite;
  WeaponData m_lastWeaponData;

};

#endif
