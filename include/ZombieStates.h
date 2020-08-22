#ifndef ZOMBIE_STATES_H_INCLUDED
#define ZOMBIE_STATES_H_INCLUDED

class ZombieIdle: public StateBase {
public:

  virtual void onEnter(ECSContext& context, Entity player);
  virtual void update(ECSContext& context, Entity player, real deltaTime);

};

#endif
