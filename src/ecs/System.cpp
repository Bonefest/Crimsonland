#include "ecs/System.h"
#include "PlayerStates.h"
#include "ZombieStates.h"
#include "Utils.h"

#include <fstream>


static void generateEffect(EffectType type, vec2 position,
                           real scale, real angle, real lifetime,
                           bool fadeout) {
  Message msg;
  msg.type = int(MessageType::SPAWN_EFFECT);
  msg.effect_info.type = type;
  msg.effect_info.x = position.x;
  msg.effect_info.y = position.y;
  msg.effect_info.scale = scale;
  msg.effect_info.angle = angle;
  msg.effect_info.lifetime = lifetime;
  msg.effect_info.fadeOut = fadeout;

  notify(msg);

}


static Entity getPlayer(Registry* registry, Bitfield components) {

  auto players = registry->findEntities(components);
  Assert(!players.empty());
  return players.front();

}


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

  registerMethod<PlayerSystem>(int(MessageType::ZOMBIE_ATTACK),
                               &PlayerSystem::onZombieAttack,
                               this);

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
  attributes->regenSpeed = context.data.regenSpeed;


  Physics* physics = new Physics();
  physics->mass = 100.0f;
  physics->size = 32.0f;
  physics->maxSpeed = context.data.maxPlayerSpeed;

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

  playerComponent->stateController = new StateController();
  playerComponent->stateController->setState<PlayerIdle>(context, player);

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
  WeaponData result {};
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
  Entity player = getPlayer(context.registry, m_playerBitfield);

  Model* model = registry->getComponent<Model>(player, ComponentID::Model);
  Physics* physics = registry->getComponent<Physics>(player, ComponentID::Physics);
  Player* playerComponent = registry->getComponent<Player>(player, ComponentID::Player);
  Transformation* transf = registry->getComponent<Transformation>(player, ComponentID::Transformation);
  Attributes* attributes = registry->getComponent<Attributes>(player, ComponentID::Attributes);

  transf->angle = getPlayerViewDirection();

  vec2 heading = degToVec(transf->angle);
  vec2 side = heading.perp();

  real playerSpeed = context.data.maxPlayerSpeed;
  if(attributes->stamina / attributes->maxStamina < 0.35f) {
    playerSpeed *= 0.4f;
  } 

  physics->maxSpeed = playerSpeed;

  vec2 acceleration;
  if(isKeyPressed(FRKey::LEFT)) {
    acceleration -= side * playerSpeed;
  }
  if(isKeyPressed(FRKey::RIGHT)) {
    acceleration += side * playerSpeed * 0.75f;
  }
  if(isKeyPressed(FRKey::UP)) {
    acceleration += heading * playerSpeed * 0.75f;
  }
  if(isKeyPressed(FRKey::DOWN)) {
    acceleration -= heading * playerSpeed * 0.25f;
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
        playerComponent->stateController->setState<PlayerIdle>(context, player);
      } else {
        playerComponent->stateController->setState<PlayerMove>(context,  player);
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

  playerComponent->stateController->update(context, player, deltaTime);

  bool playerAttacked = false;
  for(auto msg: m_unprocessedZombieAttacks) {
    vec2 attackPosition = vec2(msg.attack_info.x, msg.attack_info.y);
    vec2 vecToPlayer = transf->position - attackPosition;
    real distanceToPlayer = vecToPlayer.length();
    if(distanceToPlayer > 0.01) {
      vecToPlayer.x /= distanceToPlayer;
      vecToPlayer.y /= distanceToPlayer;
    }

    if(distanceToPlayer < physics->size * 3.0f &&
       vecToPlayer.dot(degToVec(msg.attack_info.angle)) > cos(degToRad(45.0f))) {
      attributes->health -= msg.attack_info.damage;

      generateEffect(EffectType::BLOOD, transf->position, 1.0f, transf->angle, 3.0f, true);
      generateEffect(EffectType::BLOODPRINT, transf->position, 1.0f, transf->angle, 7.0f, true);

      playerAttacked = true;
    }

  }

  m_unprocessedZombieAttacks.clear();

  if(!playerAttacked) {
    attributes->health = std::min(attributes->health + attributes->regenSpeed * deltaTime,
                                  attributes->maxHealth);
  }

  if(physics->idling) {
    attributes->stamina = std::min(attributes->stamina + attributes->regenSpeed * deltaTime,
                                   attributes->maxStamina);
  } else {
    attributes->stamina = std::max(attributes->stamina - 6.0f * deltaTime, 0.0f);
  }

}


void PlayerSystem::checkCurrentWeapon(Player* player) {
  auto& weapons = player->weapons;
  while((weapons[player->currentWeaponIndex].ammo <= 0 &&
         weapons[player->currentWeaponIndex].availableClips <= 0) &&
        weapons[player->currentWeaponIndex].type != WeaponType::KNIFE) {
    player->currentWeaponIndex = (player->currentWeaponIndex + 1) % weapons.size();
  }
}


void PlayerSystem::onWeaponPickup(Message message) {

}

void PlayerSystem::onPowerupPickup(Message message) {

}

void PlayerSystem::onZombieAttack(Message message) {
  m_unprocessedZombieAttacks.push_back(message);
}

void PlayerSystem::onMouseWheel(Message message) {
  m_lastFrameMouseWheel = message.wheel.y;
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

void ZombieSystem::init(ECSContext& context) {
  m_zombieComponents = buildBitfield(ComponentID::Model,
                                     ComponentID::Transformation,
                                     ComponentID::Physics,
                                     ComponentID::Zombie);

  Registry* registry = context.registry;
  Entity zombie = registry->createEntity();

  Model* model = new Model();
  model->sprite = createSprite("zombie_idle");
  model->alpha = 255;

  Transformation* transf = new Transformation();
  transf->position = vec2(300.0f, 300.0f);
  transf->angle = 0.0f;

  Physics* physics = new Physics();
  physics->size = 20.0f;
  physics->maxSpeed = 50.0f;

  Zombie* zombieComponent = new Zombie();
  zombieComponent->wanderingTarget = transf->position;
  zombieComponent->fov = cos(degToRad(45.0f));
  zombieComponent->hearingDistance = 50.0f;
  zombieComponent->attackDistance = 70.0f;
  zombieComponent->followingDistance = 250.0f;
  zombieComponent->sawPlayerRecently = false;
  zombieComponent->stateController = new StateController();

  Attributes* attributes = new Attributes();
  attributes->maxHealth = 500.0f;
  attributes->health = 500.0f;
  attributes->damage = 20.0f;

  registry->addComponent(zombie, model);
  registry->addComponent(zombie, transf);
  registry->addComponent(zombie, physics);
  registry->addComponent(zombie, zombieComponent);
  registry->addComponent(zombie, attributes);

  zombieComponent->stateController->setState<ZombieIdle>(context, zombie);
}

void ZombieSystem::update(ECSContext& context, real deltaTime) {
  Registry* registry = context.registry;

  Bitfield playerComponents = buildBitfield(ComponentID::Transformation,
                                            ComponentID::Physics,
                                            ComponentID::Player,
                                            ComponentID::Attributes);


  Entity player = getPlayer(registry, playerComponents);
  Transformation* playerTransform = registry->getComponent<Transformation>(player,
                                                                           ComponentID::Transformation);
  Physics* playerPhysics = registry->getComponent<Physics>(player, ComponentID::Physics);
  Player* playerComponent = registry->getComponent<Player>(player, ComponentID::Player);

  vec2 playerDirection = degToVec(playerTransform->angle);

  std::list<Entity> proceededZombies;

  // TODO(mizofix): calculate fov based on player alpha
  auto zombies = registry->findEntities(m_zombieComponents);
  for(auto zombie: zombies) {
    Transformation* zombieTransform = registry->getComponent<Transformation>(zombie,
                                                                             ComponentID::Transformation);
    Zombie* zombieComponent = registry->getComponent<Zombie>(zombie, ComponentID::Zombie);
    Physics* zombiePhysics = registry->getComponent<Physics>(zombie, ComponentID::Physics);
    Attributes* zombieAttributes = registry->getComponent<Attributes>(zombie, ComponentID::Attributes);

    if(zombieAttributes->health <= 0.0f) {
      proceededZombies.push_back(zombie);
      generateEffect(EffectType::ZOMBIE_DEATH,
                     zombieTransform->position,
                     1.0f, zombieTransform->angle, 15.0f,
                     true);
      continue;
    }

    zombieComponent->stateController->update(context, zombie, deltaTime);

    vec2 vecToPlayer = playerTransform->position - zombieTransform->position;
    real distanceToPlayer = vecToPlayer.length();
    if(distanceToPlayer > 0.01) {
      vecToPlayer.x /= distanceToPlayer;
      vecToPlayer.y /= distanceToPlayer;
    }

    if(zombieComponent->attacking) {
      zombieTransform->angle = vecToDeg(vecToPlayer);
      continue;
    }


    if(zombieComponent->sawPlayerRecently && distanceToPlayer  > zombieComponent->followingDistance) {
      zombieComponent->sawPlayerRecently = false;
      zombiePhysics->velocity = vec2();
      zombieComponent->stateController->setState<ZombieIdle>(context, zombie);
    }
    else if(zombieComponent->sawPlayerRecently) {
      // NOTE(mizofix): if zombie close enough to attack a player
      if(distanceToPlayer < zombieComponent->attackDistance) {
        zombieComponent->stateController->setState<ZombieAttack>(context, zombie);
        zombiePhysics->velocity = vec2();
      }
      else {
        // TODO(mizofix): predict player path
        zombiePhysics->velocity = vecToPlayer * zombiePhysics->maxSpeed;
        zombieTransform->angle = vecToDeg(vecToPlayer);

      }

    } else {

      real relativeDirection = degToVec(zombieTransform->angle).dot(vecToPlayer);

      // NOTE(mizofix): if zombie hears or see a player
      if(distanceToPlayer < zombieComponent->hearingDistance ||
         (relativeDirection >= zombieComponent->fov &&
          distanceToPlayer < zombieComponent->followingDistance)) {

        zombieComponent->sawPlayerRecently = true;

      }

      else {

        vec2 vecToTarget = zombieComponent->wanderingTarget - zombieTransform->position;
        real distanceToTarget = vecToTarget.length();
        vecToTarget.x /= distanceToTarget;
        vecToTarget.y /= distanceToTarget;

        if(distanceToTarget > 50.0f) {
          zombiePhysics->velocity = vecToTarget * zombiePhysics->maxSpeed;
          zombieTransform->angle = vecToDeg(vecToTarget);
        } else {
          zombieComponent->wanderingElapsedTime += deltaTime;
          if(zombieComponent->wanderingElapsedTime > 2.5f) {
              real rndX = randomReal(-200.0f, 200.0f);
              real rndY = randomReal(-200.0f, 200.0f);
              zombieComponent->wanderingTarget = zombieTransform->position + vec2(rndX, rndY);
              if(zombieComponent->wanderingTarget.x < -context.data.mapWidth * 0.5f) {
                zombieComponent->wanderingTarget.x = -context.data.mapWidth * 0.5f + randomReal(0.0f, 200.0f);
              }
              else if(zombieComponent->wanderingTarget.x > context.data.mapWidth * 0.5f) {
                zombieComponent->wanderingTarget.x = context.data.mapWidth * 0.5f - randomReal(0.0f, 200.0f);
              }

              if(zombieComponent->wanderingTarget.y < -context.data.mapHeight * 0.5f) {
                zombieComponent->wanderingTarget.y = -context.data.mapHeight * 0.5f + randomReal(0.0f, 200.0f);
              }
              else if(zombieComponent->wanderingTarget.y > context.data.mapHeight * 0.5f) {
                zombieComponent->wanderingTarget.y = context.data.mapHeight * 0.5f - randomReal(0.0f, 200.0f);
              }

              zombieComponent->wanderingElapsedTime = 0.0f;
          }
        }

      }

    }
 
  }

  for(auto zombie: proceededZombies) {
    registry->destroyEntity(zombie);
  }


}

void LevelSystem::update(ECSContext& context, real deltaTime) {

  Registry* registry = context.registry;

  Bitfield playerBitfield = buildBitfield(ComponentID::Transformation,
                                          ComponentID::Player);

  Entity player = getPlayer(registry, playerBitfield);

  Transformation* playerTransf = context.registry->getComponent<Transformation>(player,
                                                                                ComponentID::Transformation);


  context.data.roundData.elapsedTime += deltaTime;

  if(context.data.roundData.intermissionActivated) {
    if(context.data.roundData.elapsedTime > 20.0f) {
      // notify round has begun
      context.data.roundData.intermissionActivated = false;
      context.data.roundData.elapsedTime = 0.0f;
    }
  }
  else {

    Bitfield zombieComponents = buildBitfield(ComponentID::Zombie,
                                              ComponentID::Attributes,
                                              ComponentID::Transformation);

    auto zombies = registry->findEntities(zombieComponents);

    int currentRound = context.data.roundData.currentRoundNumber;
    real roundLength = currentRound * 30.0f + 60.0f;

    if(context.data.roundData.elapsedTime > roundLength) {

      context.data.roundData.intermissionActivated = true;
      context.data.roundData.elapsedTime = 0.0f;
      context.data.roundData.currentRoundNumber++;

      for(auto zombie: zombies) {

        Attributes* zombieAttributes = registry->getComponent<Attributes>(zombie,
                                                                          ComponentID::Attributes);

        zombieAttributes->health = -100.0f;

      }

    } else {

      int zombiesMaxCount = currentRound * 10 + 25;
      
      if(zombies.size() < zombiesMaxCount) {
        real zombieSpawnTime = std::max(1.0f - real(currentRound) * 0.1f, 0.5f);
        if(m_elapsedTimeFromLastGeneration < zombieSpawnTime) {
          generateZombie(context, playerTransf->position);
          m_elapsedTimeFromLastGeneration = 0.0f;
        }
      }

      for(auto zombie: zombies) {
        Transformation* zombieTransf = registry->getComponent<Transformation>(zombie,
                                                                              ComponentID::Transformation);

        if(playerTransf->position.distance(zombieTransf->position) > 2000.0f) {
          registry->destroyEntity(zombie);
        }

      }

    }

  }



}

void LevelSystem::generateZombie(ECSContext& context, const vec2& playerPos) {

  Registry* registry = context.registry;
  Entity zombie = registry->createEntity();

  real currentRound = context.data.roundData.currentRoundNumber;

  Model* model = new Model();
  model->sprite = createSprite("zombie_idle");
  model->alpha = int(randomReal(200.0f, 255.0f));

  Transformation* transf = new Transformation();

  vec2 zombiePosition = playerPos;
  while((zombiePosition - playerPos).sqLength() < context.data.windowWidth * context.data.windowHeight) {
    zombiePosition.x = randomReal(-context.data.windowWidth, context.data.windowWidth) + playerPos.x;
    zombiePosition.y = randomReal(-context.data.windowHeight, context.data.windowHeight) + playerPos.y;
  }

  transf->position = zombiePosition;
  transf->angle = randomReal(0.0f, 360.0f);
  transf->scale = randomReal(0.8f, 1.2f);

  Physics* physics = new Physics();
  physics->size = 15.0f * transf->scale;
  physics->maxSpeed = 50.0f + 10.0f * currentRound;

  Zombie* zombieComponent = new Zombie();
  zombieComponent->wanderingTarget = transf->position;
  zombieComponent->fov = cos(degToRad(std::min(45.0f + 5.0f * currentRound, 160.0f)));
  zombieComponent->hearingDistance = 50.0f + 35.0f * currentRound;
  zombieComponent->attackDistance = 70.0f;
  zombieComponent->followingDistance = 350.0f + 50.0f * currentRound;
  zombieComponent->sawPlayerRecently = false;
  zombieComponent->stateController = new StateController();

  Attributes* attributes = new Attributes();
  attributes->maxHealth = 100.0f + 100.0f * currentRound;
  attributes->health = attributes->maxHealth;
  attributes->damage = 5.0f + 2.5f * currentRound;

  registry->addComponent(zombie, model);
  registry->addComponent(zombie, transf);
  registry->addComponent(zombie, physics);
  registry->addComponent(zombie, zombieComponent);
  registry->addComponent(zombie, attributes);

  zombieComponent->stateController->setState<ZombieIdle>(context, zombie);  

}

void FootprintGenerationSystem::update(ECSContext& context, real deltaTime) {

  Registry* registry = context.registry;

  Bitfield desiredComponents = buildBitfield(ComponentID::Transformation,
                                             ComponentID::Attributes,
                                             ComponentID::Physics);

  auto entities = registry->findEntities(desiredComponents);
  for(auto entity: entities) {
    Transformation* transf = context.registry->getComponent<Transformation>(entity,
                                                                            ComponentID::Transformation);
    Attributes* attributes = context.registry->getComponent<Attributes>(entity,
                                                                        ComponentID::Attributes);

    Physics* physics = context.registry->getComponent<Physics>(entity, ComponentID::Physics);

    if(!physics->idling) {

      attributes->footprintElapsedTime += deltaTime;

      EffectType type = EffectType::FOOTPRINT;
      real printTime = 0.5f;
      if((attributes->health / attributes->maxHealth) < 0.5) {
        type = EffectType::BLOODPRINT;
        printTime = 0.35f;
      }

      if(attributes->footprintElapsedTime > printTime) {
        generateEffect(type, transf->position, 1.0f, transf->angle, 7.0f, true);
        attributes->footprintElapsedTime = 0.0f;
      }

    }

  }

}

void ModelRenderingSystem::draw(ECSContext& context) {

  Registry* registry = context.registry;

  Bitfield desiredComponents = buildBitfield(ComponentID::Model,
                                             ComponentID::Transformation);

  auto entities = registry->findEntities(desiredComponents);

  for(auto entity: entities) {
    Model* model = registry->getComponent<Model>(entity, ComponentID::Model);
    Transformation* transf = registry->getComponent<Transformation>(entity, ComponentID::Transformation);

    drawSprite(model->sprite, round(transf->position.x), round(transf->position.y),
               model->alpha, round(transf->scale), transf->angle);

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
    if(newSpeed <= previousSpeed && newSpeed < 0.01 && !physics->idling) {
      physics->velocity = vec2();
      physics->transition = true;
      physics->idling = true;
    }
    else {
      if(newSpeed >= previousSpeed && newSpeed > 0.01 && physics->idling) {
        physics->transition = true;
        physics->idling = false;
      }

      if(newSpeed > physics->maxSpeed) {
        newVelocity.x = (newVelocity.x / newSpeed) * physics->maxSpeed;
        newVelocity.y = (newVelocity.y / newSpeed) * physics->maxSpeed;
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
  case EffectType::BLOOD:
  {
    effectName = ((rand() % 2) ? "blood_1" : "blood_2");
  } break;
  case EffectType::FOOTPRINT: effectName = "footprint"; break;
  case EffectType::BLOODPRINT:{
    effectName = ((rand() % 2) ? "bloodprint_1" : "bloodprint_2");
  } break;
  case EffectType::GUN_EXPLOSION: effectName = "gun_explosion"; break;
  case EffectType::ZOMBIE_DEATH:
  {
    effectName = ((rand() % 2) ? "zombie_death1" :  "zombie_death2");
  } break;
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
    if(registry->hasComponent(collision.collision_info.entityA, ComponentID::Bullet) ||
       registry->hasComponent(collision.collision_info.entityA, ComponentID::Player) ||
       registry->hasComponent(collision.collision_info.entityB, ComponentID::Bullet) ||
       registry->hasComponent(collision.collision_info.entityB, ComponentID::Player)) {
      continue;
    }

    Transformation* transfA = registry->getComponent<Transformation>(collision.collision_info.entityA,
                                                                     ComponentID::Transformation);

    Physics* physicsA = registry->getComponent<Physics>(collision.collision_info.entityA,
                                                        ComponentID::Physics);

    Transformation* transfB = registry->getComponent<Transformation>(collision.collision_info.entityB,
                                                                     ComponentID::Transformation);
    Physics* physicsB = registry->getComponent<Physics>(collision.collision_info.entityB,
                                                        ComponentID::Physics);


    real distance = (transfB->position - transfA->position).length();
    real totalSize = physicsA->size + physicsB->size;
    vec2 direction = (transfB->position - transfA->position);

    if(distance > 0.01) {
      direction.x /= distance;
      direction.y /= distance;

      // TODO(mizofix): penetration based on mass
      transfA->position -= direction * ((totalSize - distance) * 0.5f);
      transfB->position += direction * ((totalSize - distance) * 0.5f);

      // physicsA->velocity = direction * (-physicsA->maxSpeed);
      // physicsB->velocity = direction * physicsB->maxSpeed;
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
      Attributes* zombieAttributes = registry->getComponent<Attributes>(object, ComponentID::Attributes);
      zombieAttributes->health -= bulletComponent->damage;

      Transformation* zombieTransf = registry->getComponent<Transformation>(object, ComponentID::Transformation);

      generateEffect(EffectType::BLOOD, zombieTransf->position, 1.0f, zombieTransf->angle, 3.0f, true);
      generateEffect(EffectType::BLOODPRINT, zombieTransf->position, 1.0f, zombieTransf->angle, 7.0f, true);

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
  destroySprite(m_radarSprite);
  destroySprite(m_arrowSprite);
  destroySprite(m_xiconSprite);
}

void UIRenderingSystem::init(ECSContext& context) {

  m_radarSprite = createSprite("ui_radar");
  setSpriteAnchorPoint(m_radarSprite, 1.0f, 0.0f);
  m_weaponSprite = createSprite("ui_knife");
  setSpriteAnchorPoint(m_weaponSprite, 0.0f, 0.0f);
  m_arrowSprite = createSprite("ui_arrow");
  m_xiconSprite = createSprite("ui_xicon");
}


void UIRenderingSystem::update(ECSContext& context, real deltaTime) {

  Registry* registry = context.registry;
  Bitfield desiredComponents = buildBitfield(ComponentID::Player);
  Entity player = getPlayer(registry, desiredComponents);

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

  Registry* registry = context.registry;

  Bitfield desiredComponents = buildBitfield(ComponentID::Player, ComponentID::Attributes);
  Entity player = getPlayer(registry, desiredComponents);
  Player* playerComponent = registry->getComponent<Player>(player, ComponentID::Player);
  Attributes* playerAttributes = registry->getComponent<Attributes>(player, ComponentID::Attributes);
  Transformation* playerTransform = registry->getComponent<Transformation>(player, ComponentID::Transformation);

  const auto& weaponData = playerComponent->weapons[playerComponent->currentWeaponIndex];

  char weaponInfoBuffer[255];
  sprintf(weaponInfoBuffer, "Ammo: %d / %d ( %d clips)",
          weaponData.ammo, weaponData.clipSize, weaponData.availableClips);

  int alpha = 192;
  int wpOffsetX = 10, wpOffsetY = 10, wpWidth, wpHeight;
  real wpScale = 0.5f;
  getSpriteSize(m_weaponSprite, wpWidth, wpHeight);

  // NOTE(mizofix): Weapon icon rendering
  drawText(weaponInfoBuffer, 10, 10, 0.0f, 0.0f, 0, 0, 0, false);
  drawSprite(m_weaponSprite, wpOffsetX, wpOffsetY, alpha, wpScale, 0.0f, false);

  // NOTE(mizofix): Health bar rendering
  int barAlpha = 96;
  int barWidth = 200, barHeight = 15;
  int hpBarTextOffsetY = wpOffsetY + int(wpHeight * 0.5f) - 25;
  drawText("Health points", wpOffsetX, hpBarTextOffsetY, 0.0f, 0.0f, 0, 0, 0, false);

  int hpBarOffsetY = hpBarTextOffsetY + 24;
  int hpBarWidth = int(std::max(playerAttributes->health / playerAttributes->maxHealth, 0.0f) * barWidth);
  drawRect(wpOffsetX, hpBarOffsetY, hpBarWidth, barHeight,
           108, 108, 108, barAlpha, false);

  // NOTE(mizofix): Stamina bar rendering
  int staminaBarTextOffsetY = hpBarOffsetY + barHeight + 10;
  drawText("Stamina points", wpOffsetX, staminaBarTextOffsetY, 0.0f, 0.0f, 0, 0, 0, false);

  int staminaBarOffsetY = staminaBarTextOffsetY + 24;
  int staminaBarWidth = int(std::max(playerAttributes->stamina / playerAttributes->maxStamina, 0.0f) * barWidth);
  drawRect(wpOffsetX, staminaBarOffsetY, staminaBarWidth, barHeight,
           162, 162, 162, barAlpha, false);

  // NOTE(mizofix): Radar rendering

  int radarWidth, radarHeight;
  getSpriteSize(m_radarSprite, radarWidth, radarHeight);

  int radarOffsetX = 20;
  int radarOffsetY = 20;
  int radarRadius = int(round(real(radarWidth) * 0.1f));

  vec2 radarGlobalCenter = vec2(context.data.windowWidth - radarOffsetX - radarRadius,
                                radarOffsetY + radarRadius);

  
  drawSprite(m_radarSprite, int(context.data.windowWidth) - radarOffsetX, radarOffsetY,
             128, 0.2f, 0.0f, false);


  Bitfield zombieComponents = buildBitfield(ComponentID::Transformation,
                                            ComponentID::Zombie);

  auto zombies = registry->findEntities(zombieComponents);
  for(auto zombie: zombies) {
    Transformation* zombieTransform = registry->getComponent<Transformation>(zombie,
                                                                             ComponentID::Transformation);
    vec2 vecToZombie = zombieTransform->position - playerTransform->position;
    real distanceToZombie = vecToZombie.length();
    real maxRadarDistance = 1500.0f;

    if(distanceToZombie > 0.01) {
      vecToZombie.x /= distanceToZombie;
      vecToZombie.y /= distanceToZombie;
    }
      

    distanceToZombie = std::min(distanceToZombie, maxRadarDistance);

    real convertedDistance = (distanceToZombie / maxRadarDistance) * radarRadius;

    vec2 convertedPosition = radarGlobalCenter + vecToZombie * convertedDistance;
    drawSprite(m_xiconSprite, int(convertedPosition.x), int(convertedPosition.y),
               128, 1.0f, zombieTransform->angle, false);


  }

  drawSprite(m_arrowSprite, int(radarGlobalCenter.x), int(radarGlobalCenter.y),
             128, 1.0f, playerTransform->angle, false);


}
