#include "PlayerStates.h"

void StateBase::setOwner(StateController* owner) {
  m_owner = owner;
}
