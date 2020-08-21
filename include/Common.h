#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <stdint.h>

#ifndef NDEBUG
#define info(...) fprintf(stderr, __VA_ARGS__)
#else
#define info(...)
#endif

#include "Math.h"

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

  uint32_t maxEffectsNumber;
};

enum class WeaponType {
    FISTS,
    KNIFE,
    PISTOL,
    SHOTGUN,
    RIFLE,
    CHAINGUN
};


struct WeaponData {
  WeaponType type;
  int        maxAmmo;
  int        ammo;

  int        bulletSize;
  int        durability;
  real       lifetime;
  real       damage;
  real       speed;
};


using Entity = uint32_t;

namespace Constants {

  const static Entity INVALID_ENTITY = uint32_t(-1);

};
#endif
