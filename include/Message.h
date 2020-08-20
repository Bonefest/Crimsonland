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
  SPAWN_BLOOD_EFFECT,
  WEAPON_PICKUP,
  POWERUP_PICKUP,
  CUSTOM_MESSAGE,

  // ECS

  ECS_ENTITY_DELETE,
  ECS_ENTITY_CREATED,

  COUNT
};

enum class WeaponType {
  FISTS,
  PISTOL,
  SHOTGUN,
  RIFLE,
  CHAINGUN
};

class Registry;

struct Message {
  Message(int inType = 0): type(inType) { }

  int type;
  union {

    struct {
      union {
        Bullet* blt;
        Zombie* zombie;
      } bullet_collision_info;

      union {

        Zombie* zombie;

      } dead_zombie_info;

      union {
        real x;
        real y;
        int bloodType;
      } spawn_blood_info;

      union {
        WeaponType weaponType;
      } weapon_info;

      union {
        Registry* registry;
        Entity entity;
      } entity_info;

      void* custom_data;

    };

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
