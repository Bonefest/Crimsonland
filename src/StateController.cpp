#include "StateController.h"

StateController::StateController(): m_currentState(nullptr) { }
StateController::~StateController() {
  for(auto statePair: m_states) {
    if(statePair.second != nullptr) {
      delete statePair.second;
    }
  }
}

void StateController::update(ECSContext& context, Entity target, real deltaTime) {
  if(m_currentState != nullptr) {
    m_currentState->update(context, target, deltaTime);
  }
}
