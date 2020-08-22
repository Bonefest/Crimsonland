#ifndef ZOMBIE_STATES_H_INCLUDED
#define ZOMBIE_STATES_H_INCLUDED

#include "StateBase.h"

class ZombieIdle: public StateBase {
 public:

  virtual void onEnter(ECSContext& context, Entity zombie);
  virtual void update(ECSContext& context, Entity zombie, real deltaTime);

};

class ZombieWalk: public StateBase {
 public:

  virtual void onEnter(ECSContext& context, Entity zombie);
  virtual void update(ECSContext& context, Entity zombie, real deltaTime);

};

class ZombieAttack: public StateBase {
 public:

  virtual void onEnter(ECSContext& context, Entity zombie);
  virtual void update(ECSContext& context, Entity zombie, real deltaTime);

};

#endif
