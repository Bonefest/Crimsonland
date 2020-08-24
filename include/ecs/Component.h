#ifndef COMPONENT_H_INCLUDED
#define COMPONENT_H_INCLUDED


enum class ComponentID {
  Model,
  Transformation,
  Physics,
  Attributes,
  Player,
  Zombie,
  Effect,
  Bullet,
  Trail,
  Weapon,
  Powerup,
  COUNT
};

struct Component {
  virtual ~Component() { }
  virtual ComponentID getID() = 0;
};

Component* componentFactory(ComponentID component);


#endif
