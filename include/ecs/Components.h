#ifndef COMPONENT_H_INCLUDED
#define COMPONENT_H_INCLUDED

#include <vector>
#include <list>

#include "Math.h"
#include "Common.h"
#include "Framework.h"

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

struct Model: Component {

 Model(const char* animationName):Model() {
   sprite = createSprite(animationName);
 }

 Model(): sprite(nullptr), alpha(255) { }

  ~Model() {
    if(sprite != nullptr) {
      destroySprite(sprite);
    }
  }

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

 Attributes():  damage(0.0f),
                maxHealth(0.0f),
                health(0.0f),
                maxStamina(0.0f),
                stamina(0.0f),
                maxSpeed(0.0f),
                level(1) { }


  virtual ComponentID getID() {
    return ComponentID::Attributes;
  }

  real damage;

  real regenSpeed;
  real maxHealth;
  real health;

  real maxStamina;
  real stamina;

  real maxSpeed;

  int  level;

  real footprintElapsedTime;
};

struct Physics: Component {

  Physics(): mass(1.0f), damping(1.0f) { }

  ComponentID getID() {
    return ComponentID::Physics;
  }

  // TODO(mizofix): change to MKS system of units
  vec2 acceleration;
  vec2 velocity;

  real mass;
  real damping;
  real maxSpeed;

  real size;

  // NOTE(mizofix): true if an entity began or stop moving
  // during last frame
  bool transition;
  bool idling;
};

struct TrailParticle {
  vec2 position;
  vec2 velocity;

  real elapsedTime;
};

struct Trail: Component {

  Trail(Entity inTarget, real inLifetime,
        real inMaxRandAngle, real inMaxSpeed,
        int inSize): target(inTarget),
                     lifetime(inLifetime),
                     maxRandomAngle(inMaxRandAngle),
                     maxSpeed(inMaxSpeed),
                     size(inSize){ }


  ComponentID getID() {
    return ComponentID::Trail;
  }

  Entity target;

  real lifetime;
  // NOTE(mizofix): this value is used to generate a random movement
  // direction for each particle
  real maxRandomAngle;
  real maxSpeed;
  int  size;

  std::list<TrailParticle> particles;
};

struct Bullet: Component {

  Bullet(real inDamage = 0.0f, int inDurability = 0): damage(inDamage),
                                                      durability(inDurability) { }

  ComponentID getID() {
    return ComponentID::Bullet;
  }

  real damage;
  real lifetime;
  real elapsedTime;

  // NOTE(mizofix): some bullets can move through several zombies
  int durability;

};

#include "StateController.h"

class StateController;


struct Zombie: Component {

  Zombie(): fov(0.0f),
            hearingDistance(0.0f),
            attackDistance(0.0f),
            followingDistance(0.0f),
            sawPlayerRecently(false),
            attacking(false) { }

  ~Zombie();

  virtual ComponentID getID() {
    return ComponentID::Zombie;
  }

  StateController* stateController;

  // NOTE(mizofix): fov stores as a cos value
  vec2             wanderingTarget;
  real             wanderingElapsedTime;

  real             fov;
  real             hearingDistance;
  real             attackDistance;
  real             followingDistance;
  bool             sawPlayerRecently;
  bool             attacking;
};



struct Player: Component {

  Player(): stateController(nullptr) { }
  ~Player();

  ComponentID getID() {
    return ComponentID::Player;
  }

  std::size_t             currentWeaponIndex;
  std::vector<WeaponData> weapons;
  StateController*        stateController;
};


struct WeaponBox: Component {

  virtual ComponentID getID() {
    return ComponentID::Weapon;
  }

  WeaponType type;
  int        clips;

};


Component* componentFactory(ComponentID component);

#endif
