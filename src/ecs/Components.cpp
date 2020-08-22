#include "ecs/Components.h"

Component* componentFactory(ComponentID component) {
  switch(component) {

  default: break;
  }

  return nullptr;
}

Zombie::~Zombie() {
  if(stateController != nullptr) {
    delete stateController;
  }
}

Player::~Player() {
  if(stateController != nullptr) {
    delete stateController;
  }
}
