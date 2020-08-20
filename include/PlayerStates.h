#ifndef PLAYER_STATES_H_INCLUDED
#define PLAYER_STATES_H_INCLUDED

#include "ecs/System.h"

class PlayerStateController;

enum PlayerState {
                  Idle, Move, MeleeAttack, Shoot, Reload, COUNT
};

#include "PlayerStateController.h"


class PlayerStateBase {
public:

  virtual ~PlayerStateBase() { }

  virtual void onEnter(ECSContext& context, Entity player) { }
  virtual void update(ECSContext& context, Entity player, real deltaTime) { }
  virtual void onExit(ECSContext& context, Entity player) { }

  void setOwner(PlayerStateController* owner);

protected:
  PlayerStateController* m_owner;
};

class PlayerIdle: public PlayerStateBase {
public:
  virtual void onEnter(ECSContext& context, Entity player);
  virtual void update(ECSContext& context, Entity player, real deltaTime);

};

class PlayerMove: public PlayerStateBase {
public:
  virtual void onEnter(ECSContext& context, Entity player);
  virtual void update(ECSContext& context, Entity player, real deltaTime);

};

class PlayerShoot: public PlayerStateBase {
public:
  virtual void onEnter(ECSContext& context, Entity player);
  virtual void update(ECSContext& context, Entity player, real deltaTime);

private:
  bool hasAmmo(Player* player);

};

#endif
