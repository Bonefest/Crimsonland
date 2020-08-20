#ifndef COMPONENT_H_INCLUDED
#define COMPONENT_H_INCLUDED

#include "Math.h"

enum class ComponentID { Model, Transformation, Physics, Player, Zombie, Effect, Bullet, Powerup, COUNT };

struct Component {
  virtual ~Component() { }
  virtual ComponentID getID() = 0;
};

struct Transformation: Component {

  virtual ComponentID getID() {
    return ComponentID::Transformation;
  }

  vec2 position;
  real scale;

};

Component* componentFactory(ComponentID component);

#endif
