#ifndef COMPONENT_H_INCLUDED
#define COMPONENT_H_INCLUDED

#include "Math.h"
#include "Framework.h"

enum class ComponentID {  Model,
                          Transformation,
                          Physics,
                          Attributes,
                          Player,
                          Zombie,
                          Effect,
                          Bullet,
                          Powerup,
                          // HpBar ??
                          COUNT };

struct Component {
  virtual ~Component() { }
  virtual ComponentID getID() = 0;
};

struct Model: Component {

 Model(const char* animationName):Model() {
   sprite = createSprite(animationName);
   alpha = 255;
 }

 Model(): sprite(nullptr) { }

  virtual ComponentID getID() {
    return ComponentID::Model;
  }

  Sprite* sprite;
  int     alpha;

};

struct Transformation: Component {

 Transformation(): angle(0.0f), scale(1.0f) { }

  virtual ComponentID getID() {
    return ComponentID::Transformation;
  }

  vec2 position;
  real angle;
  real scale;
};

struct Attributes: Component {

 Attributes():  maxHealth(0.0f),
                health(0.0f),
                maxStamina(0.0f),
                stamina(0.0f),
                maxSpeed(0.0f),
                level(1) { }


  virtual ComponentID getID() {
    return ComponentID::Attributes;
  }

  real maxHealth;
  real health;

  real maxStamina;
  real stamina;

  real maxSpeed;

  int level;
};

struct Zombie: Component {

  virtual ComponentID getID() {
    return ComponentID::Zombie;
  }

  bool isDead;

};

Component* componentFactory(ComponentID component);

#endif
