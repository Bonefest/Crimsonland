#include "ecs/System.h"

void PlayerSystem::init(ECSContext& context) {
  registerMethod<PlayerSystem>(int(MessageType::WEAPON_PICKUP),
                               &PlayerSystem::onWeaponPickup,
                               this);

  registerMethod<PlayerSystem>(int(MessageType::POWERUP_PICKUP),
                               &PlayerSystem::onPowerupPickup,
                               this);

  m_lastFootprintElapsedTime = 0.0f;

  Entity player = context.registry->createEntity();

  Model* model = new Model("knife_idle");
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


  Physics* physics = new Physics();
  physics->mass = 100.0f;
  physics->size = 32.0f;

  Player* playerComponent = new Player();
  playerComponent->currentWeaponIndex = 0;

  WeaponData weapon = { WeaponType::KNIFE, 0, 0 };
  playerComponent->weapons.push_back(weapon);


  // TODO(mizofix): change damping based on current tile
  physics->damping = 1.0f;

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

  playerComponent->stateController = new PlayerStateController();
  playerComponent->stateController->init(context, player);
}

void PlayerSystem::update(ECSContext& context, real deltaTime) {
  // TODO(mizofix): if player is dead -> notify player dead
  Registry* registry = context.registry;
  Entity player = getPlayer(context);

  Model* model = registry->getComponent<Model>(player, ComponentID::Model);
  Physics* physics = registry->getComponent<Physics>(player, ComponentID::Physics);
  Player* playerComponent = registry->getComponent<Player>(player, ComponentID::Player);
  Transformation* transf = registry->getComponent<Transformation>(player, ComponentID::Transformation);
  transf->angle = getPlayerViewDirection();

  vec2 heading = degToVec(transf->angle);
  vec2 side = heading.perp();

  vec2 acceleration;
  if(isKeyPressed(FRKey::LEFT)) {
    acceleration -= side * context.data.maxPlayerSpeed;
  }
  if(isKeyPressed(FRKey::RIGHT)) {
    acceleration += side * context.data.maxPlayerSpeed;
  }
  if(isKeyPressed(FRKey::UP)) {
    acceleration += heading * context.data.maxPlayerSpeed;
  }
  if(isKeyPressed(FRKey::DOWN)) {
    acceleration -= heading * context.data.maxPlayerSpeed;
  }

  physics->acceleration = acceleration;
  //info("%f %f\n", transf->position.x, transf->position.y);

  if(physics->acceleration.length() < 0.01f) {
    physics->velocity = vec2();
    physics->transition = true;
    physics->idling = true;
  }

  setCameraPosition(round(transf->position.x), round(transf->position.y));

  if(!physics->idling) {
    m_lastFootprintElapsedTime += deltaTime;
    if(m_lastFootprintElapsedTime > 0.35 && !physics->idling) {
      m_lastFootprintElapsedTime = 0.0f;

      generateFootprint(transf->position, transf->angle);

    }
  } else {
    m_lastFootprintElapsedTime = 0.0f;
  }

  playerComponent->stateController->update(context, player, deltaTime);
}

void PlayerSystem::draw(ECSContext& context) {
  // TODO(mizofix): player hp-bar rendering

  Registry* registry = context.registry;
  Entity player = getPlayer(context);
  Model* model = registry->getComponent<Model>(player, ComponentID::Model);
  Transformation* transf = registry->getComponent<Transformation>(player, ComponentID::Transformation);

  drawSprite(model->sprite, round(transf->position.x), round(transf->position.y),
             model->alpha, round(transf->scale), transf->angle);


}

void PlayerSystem::onWeaponPickup(Message message) {

}

void PlayerSystem::onPowerupPickup(Message message) {

}

void PlayerSystem::generateFootprint(vec2 position, real angle) {
  Message msg;
  msg.type = int(MessageType::SPAWN_EFFECT);
  msg.effect_info.type = EffectType::FOOTPRINT;
  msg.effect_info.x = position.x;
  msg.effect_info.y = position.y;
  msg.effect_info.scale = 1.0f;
  msg.effect_info.angle = angle;
  msg.effect_info.lifetime = 5.0f;
  msg.effect_info.fadeOut = true;

  notify(msg);

}

real PlayerSystem::getPlayerViewDirection() {
  int cursorX, cursorY;
  getCursorPos(&cursorX, &cursorY);

  int screenW, screenH;
  getScreenSize(screenW, screenH);

  vec2 cursorRelativeToCenter = vec2(cursorX, cursorY);
  cursorRelativeToCenter -= vec2(screenW, screenH) * 0.5f;

  return radToDeg(vecToRad(cursorRelativeToCenter));
}

Entity PlayerSystem::getPlayer(ECSContext& context) {
  EntitiesContainer players = context.registry->findEntities(m_playerBitfield);

  Assert(!players.empty());

  return players.front();
}


void ZombieRenderingSystem::draw(ECSContext& context) {
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

void PhysicsIntegrationSystem::update(ECSContext& context, real deltaTime) {
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
      if(transf->position.x > context.data.mapWidth * 0.5f) {
        transf->position.x = context.data.mapWidth * 0.5f;
      }
      else if(transf->position.x < -context.data.mapWidth * 0.5f) {
        transf->position.x = -context.data.mapWidth * 0.5f;
      }

      if(transf->position.y > context.data.mapHeight * 0.5f) {
        transf->position.y = context.data.mapHeight * 0.5f;
      }
      else if(transf->position.y < -context.data.mapHeight * 0.5f) {
        transf->position.y = -context.data.mapHeight * 0.5f;
      }
    }

    physics->acceleration = vec2();
  }

}
void EffectsSystem::init(ECSContext& context) {
  registerMethod(int(MessageType::SPAWN_EFFECT),
                 &EffectsSystem::onSpawnEffect,
                 this);

  m_maximalEffectsNumber = context.data.maxEffectsNumber;
}

void EffectsSystem::update(ECSContext& context, real deltaTime) {
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

void EffectsSystem::draw(ECSContext& context) {
  for(auto& effect: m_effects) {
    int alpha = 255;
    if(effect.fadeOut) {
      alpha = int((1.0f - effect.elapsedTime / effect.lifetime) * 255);
    }
    drawSprite(effect.sprite, round(effect.position.x), round(effect.position.y), alpha,
               effect.scale, effect.angle);
  }
}

void EffectsSystem::onSpawnEffect(Message message) {

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
  case EffectType::FOOTPRINT: effectName = "footprint"; break;

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
