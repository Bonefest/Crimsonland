#ifndef PLAYER_STATE_CONTROLLER_H_INCLUDED
#define PLAYER_STATE_CONTROLLER_H_INCLUDED

#include "PlayerStates.h"

class PlayerStateBase;

class PlayerStateController {
public:
  PlayerStateController();
  ~PlayerStateController();

  void init(ECSContext& context, Entity player);
  void update(ECSContext& context, Entity player, real deltaTime);
  void setState(ECSContext& context, Entity player, PlayerState state);

private:
  PlayerStateBase* m_states[PlayerState::COUNT];
  PlayerStateBase* m_currentState;
};


#endif
