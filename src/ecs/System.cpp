#include "ecs/System.h"
#include "Utils.h"

#include <fstream>

void TrailSystem::update(ECSContext& context, real deltaTime) {
  
  Registry* registry = context.registry;

  Bitfield desiredComponents = buildBitfield(ComponentID::Trail);
  auto trails = registry->findEntities(desiredComponents);

  std::list<Entity> proceededTrails;

  for(auto trail: trails) {
    Trail* trailComponent = registry->getComponent<Trail>(trail, ComponentID::Trail);

    bool targetExists = registry->isEntityExists(trailComponent->target);
    if(targetExists) {
      Transformation* targetTransf = registry->getComponent<Transformation>(trailComponent->target, ComponentID::Transformation);

      Physics* targetPhysics = registry->getComponent<Physics>(trailComponent->target, ComponentID::Physics);

      Assert(targetTransf != nullptr);
      Assert(targetPhysics != nullptr);


      trailComponent->particles.push_back(generateParticle(targetPhysics->velocity,
                                                           targetTransf->position,
                                                           trailComponent->maxRandomAngle,
                                                           trailComponent->maxSpeed));

    }

    for(auto particleIt = trailComponent->particles.begin();
        particleIt != trailComponent->particles.end();) {

      particleIt->elapsedTime += deltaTime;
      if(particleIt->elapsedTime >= trailComponent->lifetime) {
        particleIt = trailComponent->particles.erase(particleIt);
      } else {
        particleIt->position += particleIt->velocity * deltaTime;
        particleIt++;
      }

    }

    if(!targetExists && trailComponent->particles.empty()) {
      proceededTrails.push_back(trail);
    }

  }

  for(auto trail: proceededTrails) {
    registry->destroyEntity(trail);
  }

}

void TrailSystem::draw(ECSContext& context) {
  Registry* registry = context.registry;

  Bitfield desiredComponents = buildBitfield(ComponentID::Trail);
  auto trails = registry->findEntities(desiredComponents);

  for(auto trail: trails) {
    Trail* trailComponent = registry->getComponent<Trail>(trail, ComponentID::Trail);

    for(auto& particle: trailComponent->particles) {
      int alpha = int((1.0f - particle.elapsedTime / trailComponent->lifetime) * 255.0f);
      drawRect(round(particle.position.x),round(particle.position.y),
               trailComponent->size, trailComponent->size,
               128, 128, 128, alpha);
    }
  }
}

TrailParticle TrailSystem::generateParticle(const vec2& targetVelocity, const vec2& position,
                                            real maxAngle, real maxSpeed) {
  TrailParticle newParticle;
  newParticle.position = position;
  newParticle.elapsedTime = 0.0f;

  real targetDirectionAngle = vecToDeg(targetVelocity);

  real particleDirectionAgle = targetDirectionAngle + randomReal(0.0f, maxAngle);
  real particleSpeed = randomReal(0.0f, maxSpeed);

  newParticle.velocity = degToVec(particleDirectionAgle) * particleSpeed;

  return newParticle;
}

void PlayerSystem::init(ECSContext& context) {
  registerMethod<PlayerSystem>(int(MessageType::WEAPON_PICKUP),
                               &PlayerSystem::onWeaponPickup,
                               this);

  registerMethod<PlayerSystem>(int(MessageType::POWERUP_PICKUP),
                               &PlayerSystem::onPowerupPickup,
                               this);

  registerMethod<PlayerSystem>(int(MessageType::ON_MOUSE_WHEEL),
                               &PlayerSystem::onMouseWheel,
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
  initWeapons(playerComponent);

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

void PlayerSystem::initWeapons(Player* player) {
  std::ifstream file("data/weapons.json");
  nlohmann::json parser;
  file >> parser;

  nlohmann::json weaponsParser = parser["weapons"];
  for(auto weaponIt = weaponsParser.begin();
      weaponIt != weaponsParser.end();
      weaponIt++) {

    player->weapons.push_back(parseWeapon(weaponIt.value()));

  }
}

WeaponData PlayerSystem::parseWeapon(nlohmann::json& parser) {
  WeaponData result;
  result.type = WeaponType(parser["type"]);
  result.damage = parser["damage"];

  if(result.type != WeaponType::KNIFE) {

    result.handOffset.x = parser["offset"]["x"];
    result.handOffset.y = parser["offset"]["y"];

    result.availableClips = parser.value("init_clips", 0);
    result.clipSize = parser["clip_size"];
    result.ammo = parser.value("init_ammo", 0);

    result.bulletSize = parser.value("bulletSize", 2);
    result.durability = parser.value("durability", 1);
    result.lifetime = parser["lifetime"];
    result.speed = parser["speed"];

    result.trailLifetime = parser["trail_lifetime"];
    result.trailMaxAngle = parser.value("trail_max_angle", 45.0f);
    result.trailScatterSpeed = parser.value("trail_scatter_speed", 2.0f);

    result.spriteName = parser["sprite_name"];
  }

  return result;
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
  if(m_lastFrameMouseWheel != 0) {
    std::size_t weaponsSize = playerComponent->weapons.size();
    std::size_t newWeaponIndex = (weaponsSize +
                                  playerComponent->currentWeaponIndex +
                                  m_lastFrameMouseWheel) % weaponsSize;

    if(newWeaponIndex != playerComponent->currentWeaponIndex) {

      playerComponent->currentWeaponIndex = newWeaponIndex;

      checkCurrentWeapon(playerComponent);

      if(physics->idling) {
        playerComponent->stateController->setState(context, player, PlayerState::Idle);
      } else {
        playerComponent->stateController->setState(context,  player, PlayerState::Move);
      }
    }

    m_lastFrameMouseWheel = 0;
  } else {
    checkCurrentWeapon(playerComponent);
  }



  physics->acceleration = acceleration;

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


void PlayerSystem::checkCurrentWeapon(Player* player) {
  auto& weapons = player->weapons;
  while((weapons[player->currentWeaponIndex].ammo <= 0 &&
         weapons[player->currentWeaponIndex].availableClips <= 0) &&
        weapons[player->currentWeaponIndex].type != WeaponType::KNIFE) {
    player->currentWeaponIndex = (player->currentWeaponIndex + 1) % weapons.size();
  }
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

void PlayerSystem::onMouseWheel(Message message) {
  m_lastFrameMouseWheel = message.wheel.y;
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
  case EffectType::GUN_EXPLOSION: effectName = "gun_explosion"; break;

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

void PhysicsCollisionSystem::update(ECSContext& context, real deltaTime) {
  Registry* registry = context.registry;
  Bitfield desiredComponents = buildBitfield(ComponentID::Transformation,
                                             ComponentID::Physics);

  auto entities = registry->findEntities(desiredComponents);

  Message collisionMsg;
  collisionMsg.type = int(MessageType::ON_COLLISION);

  for(auto entityAIt = entities.begin(); entityAIt != entities.end(); entityAIt++) {
    Transformation* transfA = registry->getComponent<Transformation>(*entityAIt,
                                                                     ComponentID::Transformation);

    Physics* physicsA = registry->getComponent<Physics>(*entityAIt, ComponentID::Physics);

    for(auto entityBIt = std::next(entityAIt); entityBIt != entities.end(); entityBIt++) {

      if(*entityAIt == *entityBIt) {
        continue;
      }

      Transformation* transfB = registry->getComponent<Transformation>(*entityBIt,
                                                                       ComponentID::Transformation);

      Physics* physicsB = registry->getComponent<Physics>(*entityBIt, ComponentID::Physics);

      real distanceAtoB = transfA->position.distance(transfB->position);
      if(distanceAtoB < physicsA->size + physicsB->size) {
        collisionMsg.collision_info.entityA = *entityAIt;
        collisionMsg.collision_info.entityB = *entityBIt;

        notify(collisionMsg);
      }

    }
  }
}

void PenetrationResolutionSystem::init(ECSContext& context) {
  registerMethod<PenetrationResolutionSystem>(int(MessageType::ON_COLLISION),
                                              &PenetrationResolutionSystem::onCollision,
                                              this);
}

void PenetrationResolutionSystem::update(ECSContext& context, real deltaTime) {
  Registry* registry = context.registry;

  for(auto collision: m_unprocessedCollisions) {
    Transformation* transfA = registry->getComponent<Transformation>(collision.collision_info.entityA,
                                                                     ComponentID::Transformation);

    Transformation* transfB = registry->getComponent<Transformation>(collision.collision_info.entityB,
                                                                     ComponentID::Transformation);

    real distance = (transfB->position - transfA->position).length();
    vec2 direction = (transfB->position - transfA->position);

    if(distance > 0.01) {
      direction.x /= distance;
      direction.y /= distance;

      // TODO(mizofix): penetration based on mass
      transfA->position -= direction * (distance * 0.5f);
      transfB->position += direction * (distance * 0.5f);
    }
  }

  m_unprocessedCollisions.clear();
}

void PenetrationResolutionSystem::onCollision(Message message) {
  m_unprocessedCollisions.push_back(message);
}

void BulletSystem::init(ECSContext& context) {
  registerMethod<BulletSystem>(int(MessageType::ON_COLLISION),
                               &BulletSystem::onCollision,
                               this);
}

void BulletSystem::update(ECSContext& context, real deltaTime) {

  Registry* registry = context.registry;

  for(auto collision: m_unprocessedCollisions) {
    Entity bullet = Constants::INVALID_ENTITY;
    Entity object = Constants::INVALID_ENTITY;

    if(registry->hasComponent(collision.collision_info.entityA, ComponentID::Bullet)) {
      bullet = collision.collision_info.entityA;
      object = collision.collision_info.entityB;
    }
    else if(registry->hasComponent(collision.collision_info.entityB, ComponentID::Bullet)) {
      bullet = collision.collision_info.entityB;
      object = collision.collision_info.entityA;
    }

    if(bullet != Constants::INVALID_ENTITY && registry->hasComponent(object, ComponentID::Zombie)) {
      Bullet* bulletComponent = registry->getComponent<Bullet>(bullet, ComponentID::Bullet);
      Zombie* zombie = registry->getComponent<Zombie>(object, ComponentID::Zombie);

      //zombie->health -= bulletComponent->damage; or notify?

      // TODO(mizofix): generate an effect

      bulletComponent->durability--;
      if(bulletComponent->durability <= 0) {
        registry->destroyEntity(bullet);
      }


    }

  }

  Bitfield desiredComponents = buildBitfield(ComponentID::Bullet);
  auto bullets = registry->findEntities(desiredComponents);
  for(auto bullet: bullets) {
    Bullet* bulletComponent = registry->getComponent<Bullet>(bullet, ComponentID::Bullet);
    bulletComponent->elapsedTime -= deltaTime;

    if(bulletComponent->elapsedTime > bulletComponent->lifetime) {
      registry->destroyEntity(bullet);
    }
  }

  m_unprocessedCollisions.clear();
}

void BulletSystem::onCollision(Message message) {
  m_unprocessedCollisions.push_back(message);
}

UIRenderingSystem::~UIRenderingSystem() {
  destroySprite(m_weaponSprite);
}

void UIRenderingSystem::init(ECSContext& context) {

  m_weaponSprite = createSprite("ui_knife");
  setSpriteAnchorPoint(m_weaponSprite, 0.0f, 0.0f);

}


void UIRenderingSystem::update(ECSContext& context, real deltaTime) {

  Registry* registry = context.registry;
  Bitfield desiredComponents = buildBitfield(ComponentID::Player);
  auto players = registry->findEntities(desiredComponents);

  Assert(!players.empty());

  Entity player = players.front();
  Player* playerComponent = registry->getComponent<Player>(player, ComponentID::Player);
  WeaponType currentWeapon = playerComponent->weapons[playerComponent->currentWeaponIndex].type;

  if(currentWeapon == WeaponType::KNIFE) {
    setAnimation(m_weaponSprite, "ui_knife");
  }
  else if(currentWeapon == WeaponType::PISTOL) {
    setAnimation(m_weaponSprite, "ui_pistol");
  }
  else if(currentWeapon == WeaponType::RIFLE) {
    setAnimation(m_weaponSprite, "ui_rifle");
  }
  else if(currentWeapon == WeaponType::SHOTGUN) {
    setAnimation(m_weaponSprite, "ui_shotgun");
  }

}

void UIRenderingSystem::draw(ECSContext& context) {
  drawSprite(m_weaponSprite, 10, 10, 192, 0.5f, 0.0f, false);
}
