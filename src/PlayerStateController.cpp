#include "PlayerStateController.h"


PlayerStateController::PlayerStateController() {
  for(int i = 0;i < int(PlayerState::COUNT); ++i) {
    m_states[i] = nullptr;
  }
}

PlayerStateController::~PlayerStateController() {
  for(int i = 0;i < int(PlayerState::COUNT); ++i) {
    if(m_states[i]) {
      delete m_states[i];
    }
  }
}

void PlayerStateController::init(ECSContext& context, Entity player) {
  m_states[int(PlayerState::Idle)] = new PlayerIdle();
  m_states[int(PlayerState::Move)] = new PlayerMove();
  m_states[int(PlayerState::MeleeAttack)] = new PlayerAttack();
  m_states[int(PlayerState::Shoot)] = new PlayerShoot();
  //m_states[int(PlayerState::Reload)] = new PlayerReload();

  for(int i = 0; i < int(PlayerState::COUNT); ++i) {
    if(m_states[i]) {
      m_states[i]->setOwner(this);
    }
  }

  setState(context, player, PlayerState::Idle);
}

void PlayerStateController::update(ECSContext& context, Entity player, real deltaTime) {
  if(m_currentState) {
    m_currentState->update(context, player, deltaTime);
  }
}

void PlayerStateController::setState(ECSContext& context, Entity player, PlayerState state) {
  if(m_currentState != nullptr) {
    m_currentState->onExit(context, player);
  }

  m_currentState = m_states[int(state)];
  m_currentState->onEnter(context, player);

}
