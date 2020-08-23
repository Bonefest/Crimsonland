#ifndef PLAYER_STATES_H_INCLUDED
#define PLAYER_STATES_H_INCLUDED

#include "StateBase.h"

#include "ecs/System.h"
#include "ecs/Components.h"


class Player;

class PlayerIdle: public StateBase {
public:

  virtual void onEnter(ECSContext& context, Entity player);
  virtual void update(ECSContext& context, Entity player, real deltaTime);

};

class PlayerMove: public StateBase {
public:
  virtual void onEnter(ECSContext& context, Entity player);
  virtual void update(ECSContext& context, Entity player, real deltaTime);

};

class PlayerShoot: public StateBase {
public:
  virtual void onEnter(ECSContext& context, Entity player);
  virtual void update(ECSContext& context, Entity player, real deltaTime);

private:
  Entity generateBullet(Registry* registry,
                        const vec2& direction, const vec2& position,
                        const WeaponData& data);

  void generateExplosion(const vec2& position, real angle);

  bool needToReload(Player* player);
  bool hasAvailableAmmo(Player* player);
};

class PlayerAttack: public StateBase {
public:

  virtual void onEnter(ECSContext& context, Entity player);
  virtual void update(ECSContext& context, Entity player, real deltaTime);
private:
  void generateAttack(Registry* registry, real angle, const vec2& position, real damage);

};

class PlayerReload: public StateBase {
public:
  virtual void onEnter(ECSContext& context, Entity player);
  virtual void update(ECSContext& context, Entity player, real deltaTime);

};

#endif
