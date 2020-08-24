#include "Components.h"


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
