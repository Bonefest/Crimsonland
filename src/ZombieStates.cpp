#include "ZombieStates.h"

void ZombieIdle::onEnter(ECSContext& context, Entity zombie) {
  Registry* registry = context.registry;
  Model* model = registry->getComponent<Model>(zombie, ComponentID::Model);
  setAnimation(model->sprite, "zombie_idle");
  setFrozenAnimation(model->sprite, false);
}

void ZombieIdle::update(ECSContext& context, Entity zombie, real deltaTime) {
  Registry* registry = context.registry;
  Physics* physics = registry->getComponent<Physics>(zombie, ComponentID::Physics);
  Model* model = registry->getComponent<Model>(zombie, ComponentID::Model);
  updateAnimation(model->sprite, deltaTime);

  if(!physics->idling) {
    return m_owner->setState<ZombieWalk>(context, zombie);
  }
}

void ZombieWalk::onEnter(ECSContext& context, Entity zombie) {
  Registry* registry = context.registry;
  Model* model = registry->getComponent<Model>(zombie, ComponentID::Model);

  char animationName[32];
  sprintf(animationName, "zombie_walk%d", rand() % 2 + 1);
  setAnimation(model->sprite, animationName);
}

void ZombieWalk::update(ECSContext& context, Entity zombie, real deltaTime) {
  Registry* registry = context.registry;
  Physics* physics = registry->getComponent<Physics>(zombie, ComponentID::Physics);
  Model* model = registry->getComponent<Model>(zombie, ComponentID::Model);
  updateAnimation(model->sprite, deltaTime);

  if(physics->idling) {
    return m_owner->setState<ZombieIdle>(context, zombie);
  }
}

void ZombieAttack::onEnter(ECSContext& context, Entity zombie) {
  Registry* registry = context.registry;
  Model* model = registry->getComponent<Model>(zombie, ComponentID::Model);
  Zombie* zombieComponent = registry->getComponent<Zombie>(zombie, ComponentID::Zombie);
  zombieComponent->attacking = true;

  char animationName[32];
  sprintf(animationName, "zombie_attack%d", rand() % 3 + 1);
  setAnimation(model->sprite, animationName);
}

void ZombieAttack::update(ECSContext& context, Entity zombie, real deltaTime) {
  Registry* registry = context.registry;
  Physics* physics = registry->getComponent<Physics>(zombie, ComponentID::Physics);
  Model* model = registry->getComponent<Model>(zombie, ComponentID::Model);
  Zombie* zombieComponent = registry->getComponent<Zombie>(zombie, ComponentID::Zombie);
  updateAnimation(model->sprite, deltaTime);

  if(isAnimationFinished(model->sprite)) {
    // notify attack finished
    zombieComponent->attacking = false;
    if(physics->idling) {
      return m_owner->setState<ZombieIdle>(context, zombie);
    } else {
      return m_owner->setState<ZombieWalk>(context, zombie);
    }
  }
}
