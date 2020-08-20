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

  virtual void init(ECSContext& context) { }
  virtual void update(ECSContext& context, real deltaTime) { }
  virtual void draw(ECSContext& context) { }
};

class PlayerSystem: public System {
public:
  virtual void init(ECSContext& context) {
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

    Player* playerComponent = new Player();
    playerComponent->currentWeaponIndex = 0;

    WeaponData weapon = { WeaponType::FISTS, 0, 0 };
    playerComponent->weapons.push_back(weapon);


    Physics* physics = new Physics();
    physics->mass = 100.0f;
    physics->size = 32.0f;

    // TODO(mizofix): change damping based on current tile
    physics->damping = 0.98f;

    context.registry->addComponent(player, model);
    context.registry->addComponent(player, transf);
    context.registry->addComponent(player, attributes);
    context.registry->addComponent(player, playerComponent);
    context.registry->addComponent(player, physics);

    m_playerBitfield = buildBitfield(ComponentID::Model,
                                     ComponentID::Transformation,
                                     ComponentID::Attributes,
                                     ComponentID::Player,
                                     ComponentID::Physics);
  }

  virtual void update(ECSContext& context, real deltaTime) {


    // TODO(mizofix): if player is dead -> notify player dead
    Registry* registry = context.registry;
    Entity player = getPlayer(context);

    Model* model = registry->getComponent<Model>(player, ComponentID::Model);
    Physics* physics = registry->getComponent<Physics>(player, ComponentID::Physics);
    Transformation* transf = registry->getComponent<Transformation>(player, ComponentID::Transformation);
    // TODO(mizofix): player controll

    vec2 acceleration;
    if(isKeyPressed(FRKey::LEFT)) {
      acceleration -= vec2(100.0f, 0.0f);
    }
    if(isKeyPressed(FRKey::RIGHT)) {
      acceleration += vec2(100.0f, 0.0f);
    }
    if(isKeyPressed(FRKey::UP)) {
      acceleration -= vec2(0.0f, 100.0f);
    }
    if(isKeyPressed(FRKey::DOWN)) {
      acceleration += vec2(0.0f, 100.0f);

      Message msg;
      msg.type = int(MessageType::SPAWN_EFFECT);
      msg.effect_info.type = EffectType::BLOOD_1;
      msg.effect_info.x = transf->position.x;
      msg.effect_info.y = transf->position.y;
      msg.effect_info.scale = 1.0f;
      msg.effect_info.angle = 0.0f;
      msg.effect_info.lifetime = 0.06;
      msg.effect_info.fadeOut = false;

      notify(msg);
    }

    physics->acceleration = acceleration;

    if(physics->acceleration.length() < 0.01f) {
      physics->velocity = vec2();
      physics->transition = true;
      physics->idling = true;
    }

    if(physics->transition) {
      if(physics->idling) {
        setFrozenAnimation(model->sprite, true);
      }
      else {
        setFrozenAnimation(model->sprite, false);
      }
      resetAnimation(model->sprite);
    }

    setCameraPosition(round(transf->position.x), round(transf->position.y));

    transf->angle = getPlayerViewDirection();

    updateAnimation(model->sprite, deltaTime);

  }

  virtual void draw(ECSContext& context) {
    // TODO(mizofix): player hp-bar rendering

    Registry* registry = context.registry;
    Entity player = getPlayer(context);
    Model* model = registry->getComponent<Model>(player, ComponentID::Model);
    Transformation* transf = registry->getComponent<Transformation>(player, ComponentID::Transformation);

    drawSprite(model->sprite, round(transf->position.x), round(transf->position.y),
               model->alpha, round(transf->scale), transf->angle);


  }

  void onWeaponPickup(Message message) {

  }

  void onPowerupPickup(Message message) {

  }

private:
  real getPlayerViewDirection() {
    int cursorX, cursorY;
    getCursorPos(&cursorX, &cursorY);

    int screenW, screenH;
    getScreenSize(screenW, screenH);

    vec2 cursorRelativeToCenter = vec2(cursorX, cursorY);
    cursorRelativeToCenter -= vec2(screenW, screenH) * 0.5f;

    return radToDeg(vecToRad(cursorRelativeToCenter));
  }

  Entity getPlayer(ECSContext& context) {
    EntitiesContainer players = context.registry->findEntities(m_playerBitfield);

    Assert(!players.empty());

    return players.front();

  }

  Bitfield m_playerBitfield;
};

class ZombieRenderingSystem: public System {
public:
  virtual void draw(ECSContext& context) {
    Registry* registry = context.registry;

    Bitfield desiredComponents = buildBitfield(ComponentID::Model,
                                               ComponentID::Transformation);

    EntitiesContainer entities = registry->findEntities(desiredComponents);
    for(auto entity: entities) {
      Transformation* transformation = registry->getComponent<Transformation>(entity, ComponentID::Transformation);
      Model* model = registry->getComponent<Model>(entity, ComponentID::Model);
      Attributes* attributes = registry->getComponent<Attributes>(entity, ComponentID::Attributes);
      // TODO(mizofix): draw hp bar

      // drawSprite(model->sprite,
      //            round(transformation->position.x),
      //            round(transformation->position.y),
      //            model->alpha,
      //            transformation->angle,
      //            transformation->scale);

    }
  }
};

class PhysicsIntegrationSystem: public System {
public:
  virtual void update(ECSContext& context, real deltaTime) {
    Registry* registry = context.registry;

    Bitfield desiredComponents = buildBitfield(ComponentID::Transformation,
                                               ComponentID::Physics);

    EntitiesContainer entities = registry->findEntities(desiredComponents);
    for(auto entity: entities) {
      Transformation* transf = registry->getComponent<Transformation>(entity, ComponentID::Transformation);
      Physics* physics = registry->getComponent<Physics>(entity, ComponentID::Physics);

      physics->transition = false;

      vec2 newVelocity = (physics->velocity + physics->acceleration * deltaTime) * physics->damping;
      real newSpeed = newVelocity.length();
      real previousSpeed = physics->velocity.length();
      if(newSpeed < previousSpeed && newSpeed < 0.01 && !physics->idling) {
        physics->velocity = vec2();
        physics->transition = true;
        physics->idling = true;
      }
      else {
        if(newSpeed > previousSpeed && newSpeed > 0.01 && physics->idling) {
          physics->transition = true;
          physics->idling = false;
        }

        physics->velocity = newVelocity;
        transf->position += physics->velocity * deltaTime;
      }

      physics->acceleration = vec2();
    }

  }
};


struct Effect {

  Sprite* sprite;
  vec2 position;
  real scale;
  real lifetime;
  real elapsedTime;
  real angle;
  bool fadeOut;

};

using EffectsContainer = std::list<Effect>;

class EffectsSystem: public System {
public:

  virtual void init(ECSContext& context) {
    registerMethod(int(MessageType::SPAWN_EFFECT),
                   &EffectsSystem::onSpawnEffect,
                   this);

    m_maximalEffectsNumber = context.data.maxEffectsNumber;
  }

  virtual void update(ECSContext& context, real deltaTime) {
    for(auto effectIt = m_effects.begin(); effectIt != m_effects.end();) {
      effectIt->elapsedTime += deltaTime;
      if(effectIt->elapsedTime >= effectIt->lifetime) {
        effectIt = m_effects.erase(effectIt);
      }
      else {
        updateAnimation(effectIt->sprite, deltaTime);
        effectIt++;
      }
    }
  }

  virtual void draw(ECSContext& context) {
    for(auto& effect: m_effects) {
      int alpha = int((1.0f - effect.elapsedTime / effect.lifetime) * 255);
      drawSprite(effect.sprite, round(effect.position.x), round(effect.position.y), alpha,
                 effect.scale, effect.angle);
    }
  }

  void onSpawnEffect(Message message) {

    Effect newEffect;
    newEffect.position.x = message.effect_info.x;
    newEffect.position.y = message.effect_info.y;
    newEffect.scale = message.effect_info.scale;
    newEffect.angle = message.effect_info.angle;
    newEffect.lifetime = message.effect_info.lifetime;
    newEffect.elapsedTime = 0.0f;
    newEffect.fadeOut = message.effect_info.fadeOut;

    const char* effectName = "";

    switch(message.effect_info.type) {
    case EffectType::BLOOD_1: effectName = "blood_1"; break;
    case EffectType::BLOOD_2: effectName = "blood_2"; break;
    case EffectType::BLOOD_3: effectName = "blood_3"; break;
    case EffectType::STEPS: effectName = "steps"; break;

    default: break;
    }

    newEffect.sprite = createSprite(effectName);
    if(newEffect.sprite != nullptr) {
      if(m_effects.size() > m_maximalEffectsNumber) {
        m_effects.pop_front();
      }

      setFrozenAnimation(newEffect.sprite, false);

      m_effects.push_back(newEffect);
    }
  }

private:
  EffectsContainer m_effects;
  uint32_t         m_maximalEffectsNumber;
};

#endif
