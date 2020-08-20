#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include "Common.h"
#include "Message.h"
#include "ecs/Registry.h"

struct ECSContext {
  Registry* registry;
  WorldData data;
};

class System {
public:
  virtual ~System() { }

  virtual void init(ECSContext context) { }
  virtual void update(ECSContext context, real deltaTime) { }
  virtual void draw(ECSContext context) { }
};

class PlayerSystem: public System {
public:
  virtual void init(ECSContext context) {
    registerMethod<PlayerSystem>(int(MessageType::WEAPON_PICKUP),
                                 &PlayerSystem::onWeaponPickup,
                                 this);

    registerMethod<PlayerSystem>(int(MessageType::POWERUP_PICKUP),
                                 &PlayerSystem::onPowerupPickup,
                                 this);

    Entity player = context.registry->createEntity();

    Model* model = new Model("player_walk");
    setFrozenAnimation(model->sprite, true);
    resetAnimation(model->sprite);

    // TODO(mizofix): spawn at random position
    Transformation* transf = new Transformation();
    transf->position = vec2(320, 240);

    Attributes* attributes = new Attributes();
    attributes->maxHealth = context.data.maxPlayerHealth;
    attributes->health = attributes->maxHealth;
    attributes->maxStamina = context.data.maxPlayerStamina;
    attributes->stamina = attributes->maxStamina;
    attributes->maxSpeed = context.data.maxPlayerSpeed;

    context.registry->addComponent(player, model);
    context.registry->addComponent(player, transf);
    context.registry->addComponent(player, attributes);

  }

  virtual void update(ECSContext context, real deltaTime) {
    // TODO(mizofix): player controll

    // TODO(mizofix): if player is dead -> notify player dead
  }

  virtual void draw(ECSContext context) {
    // TODO(mizofix): player hp-bar rendering

  }

  void onWeaponPickup(Message message) {

  }

  void onPowerupPickup(Message message) {

  }

private:

};

class ZombieRenderingSystem: public System {
public:
  virtual void draw(ECSContext context) {
    Registry* registry = context.registry;

    Bitfield desiredComponents = buildBitfield(ComponentID::Model,
                                               ComponentID::Transformation);

    EntitiesContainer entities = registry->findEntities(desiredComponents);
    for(auto entity: entities) {
      Transformation* transformation = registry->getComponent<Transformation>(entity, ComponentID::Transformation);
      Model* model = registry->getComponent<Model>(entity, ComponentID::Model);
      Attributes* attributes = registry->getComponent<Attributes>(entity, ComponentID::Attributes);
      // TODO(mizofix): draw hp bar

      drawSprite(model->sprite,
                 round(transformation->position.x),
                 round(transformation->position.y),
                 model->alpha,
                 transformation->angle,
                 transformation->scale);

    }
  }
};

#endif
