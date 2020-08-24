#ifndef SYSTEMS_H_INCLUDED
#define SYSTEMS_H_INCLUDED

#include "Message.h"
#include "Common.h"
#include "Assert.h"
#include "json.hpp"

#include "ecs/System.h"
#include "ecs/Components.h"

#include <list>


// NOTE(mizofix): current trail system is frame rate dependent,
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
  virtual bool init(ECSContext& context);
  virtual void update(ECSContext& context, real deltaTime);

  void onWeaponPickup(Message message);
  //void onPowerupPickup(Message message);
  void onMouseWheel(Message message);

  void onZombieAttack(Message message);
  void onCollision(Message message);

private:
  void processPlayerCollisions(Registry* registry);

  void initWeapons(Player* player);
  WeaponData parseWeapon(nlohmann::json& parser);
  void checkCurrentWeapon(Player* player);

  real getPlayerViewDirection();

  void notifyPlayerDead();

  Bitfield m_playerBitfield;
  int m_lastFrameMouseWheel;

  std::list<Message> m_unprocessedZombieAttacks;
  std::list<Message> m_unprocessedCollisions;
};

class ZombieSystem: public System {
public:
  virtual bool init(ECSContext& context);
  virtual void update(ECSContext& context, real deltaTime);

  void onGenerateZombie(Message message);
private:
  Bitfield m_zombieComponents;
};


class LevelSystem: public System {
public:
  virtual bool init(ECSContext& context);
  virtual void update(ECSContext& context, real deltaTime);

private:
  void generateZombie(ECSContext& context, const vec2& playerPos);
  void generateWeaponBox(ECSContext& context, const vec2& playerPos);
  vec2 generateRandomPosition(const vec2& playerPosition, real threshold, real radius,
                              real mapWidth, real mapHeight);

  real m_elapsedTimeFromLastZombieGeneration;
  real m_elapsedTimeFromLastBoxGeneration;

};

class NotificationSystem {
public:
  virtual bool init(ECSContext& context);
  virtual void update(ECSContext& context, real deltaTime);

};


class FootprintGenerationSystem: public System {
public:
  virtual void update(ECSContext& context, real deltaTime);

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
  virtual bool init(ECSContext& context);
  virtual void update(ECSContext& context, real deltaTime);

  void onCollision(Message message);
private:
  MessageContainer m_unprocessedCollisions;
};

class BulletSystem: public System {
public:
  virtual bool init(ECSContext& context);
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

  virtual bool init(ECSContext& context);
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

  virtual bool init(ECSContext& context);
  virtual void update(ECSContext& context, real deltaTime);
  virtual void draw(ECSContext& context);

private:
  vec2 convertToRadarCoords(const vec2& radarPosition, real radarRadius, real maxDistance,
                            const vec2& targetPosition, const vec2& playerPosition);

  Sprite*    m_radarSprite;
  Sprite*    m_weaponSprite;
  Sprite*    m_arrowSprite;
  Sprite*    m_xiconSprite;
  Sprite*    m_circleSprite;
  Sprite*    m_reticleSprite;

  real       m_lastDeltaTime;
};

#endif
