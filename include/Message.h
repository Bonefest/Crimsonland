#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include "Common.h"
#include "Math.h"
#include <functional>

class Bullet;
class Zombie;

enum class MessageType {
  BULLET_COLLISION,
  ZOMBIE_DEAD,
  SPAWN_ZOMBIE,
  SPAWN_EFFECT,
  WEAPON_PICKUP,
  POWERUP_PICKUP,
  CUSTOM_MESSAGE,

  // ECS

  ECS_ENTITY_DELETE,
  ECS_ENTITY_CREATED,

  // EVENTS

  ON_MOUSE_WHEEL,

  COUNT
};


enum class EffectType {
  BLOOD_1,
  BLOOD_2,
  BLOOD_3,
  BLOOD_RANDOM,
  BLOODPRINT,
  FOOTPRINT
};

class Registry;

struct Message {
  Message(int inType = 0): type(inType) { }

  int type;
  union {

    struct {
      Bullet* blt;
      Zombie* zombie;
    } bullet_collision_info;

    struct {

      Zombie* zombie;

    } dead_zombie_info;

    struct {
      real x;
      real y;
      int bloodType;
    } spawn_blood_info;

    struct {
      WeaponType weaponType;
    } weapon_info;

    struct {
      Registry* registry;
      Entity entity;
    } entity_info;

    struct {

      EffectType type;
      real x;
      real y;
      real scale;
      real angle;
      real lifetime;

      bool fadeOut;

    } effect_info;

    struct {
      int y;

    } wheel;

    void* custom_data;

  };

};

void addNewMessage(Message message);
bool pollMessage(Message& message);
bool peekMessage(Message& message);
bool hasMessage();

void notify(Message message);

using MessageFunction = std::function<void(Message)>;
void subscribeToMessage(int type, MessageFunction function);

template <typename T>
void registerMethod(int type, void(T::*method)(Message), T* owner) {
  auto func = std::bind(method, owner, std::placeholders::_1);
  subscribeToMessage(type, func);
}

#endif
