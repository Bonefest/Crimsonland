#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <stdint.h>
#include <string>

#include "Math.h"


#ifndef NDEBUG
#define info(...) fprintf(stderr, __VA_ARGS__)
#else
#define info(...)
#endif


struct WorldData {

  int  numEnemies;
  int  numAmmo;

  int  windowWidth;
  int  windowHeight;

  real mapWidth;
  real mapHeight;

  real maxPlayerHealth;
  real maxPlayerStamina;
  real maxPlayerSpeed;
  real regenSpeed;

  uint32_t maxEffectsNumber;
};

enum class WeaponType {
    KNIFE,
    PISTOL,
    SHOTGUN,
    RIFLE,
    CHAINGUN
};


struct WeaponData {
  WeaponType type;

  vec2       handOffset;

  int        availableClips;
  int        clipSize;
  int        ammo;

  int        bulletSize;
  int        durability;
  real       lifetime;
  real       damage;
  real       speed;

  real       trailLifetime;
  real       trailMaxAngle;
  real       trailScatterSpeed;

  std::string spriteName;
};


using Entity = uint32_t;

namespace Constants {

  const static Entity INVALID_ENTITY = uint32_t(-1);

};
#endif
