
#include "PlayerStates.h"
#include "Framework.h"

void PlayerIdle::onEnter(ECSContext& context, Entity player) {
  Model* model = context.registry->getComponent<Model>(player, ComponentID::Model);
  Player* playerComponent = context.registry->getComponent<Player>(player, ComponentID::Player);

  WeaponType currentWeapon = playerComponent->weapons[playerComponent->currentWeaponIndex].type;
  if(currentWeapon == WeaponType::KNIFE) {
    setAnimation(model->sprite, "knife_idle");
  }
  else if(currentWeapon == WeaponType::PISTOL) {
    setAnimation(model->sprite, "pistol_idle");
  }
  else if(currentWeapon == WeaponType::SHOTGUN) {
    setAnimation(model->sprite, "shotgun_idle");
  }
  else if(currentWeapon == WeaponType::RIFLE) {
    setAnimation(model->sprite, "rifle_idle");
  }

  setFrozenAnimation(model->sprite, false);
}

void PlayerIdle::update(ECSContext& context, Entity player, real deltaTime) {
  // if player began movement -> switch to moving
  Model* model = context.registry->getComponent<Model>(player, ComponentID::Model);
  Player* playerComponent = context.registry->getComponent<Player>(player, ComponentID::Player);
  Physics* physics = context.registry->getComponent<Physics>(player, ComponentID::Physics);

  updateAnimation(model->sprite, deltaTime);

  if(physics->transition && !physics->idling) {
    return m_owner->setState<PlayerMove>(context, player);
  }

  WeaponType currentWeapon = playerComponent->weapons[playerComponent->currentWeaponIndex].type;

  if(isButtonPressed(FRMouseButton::LEFT)) {
    if(currentWeapon != WeaponType::KNIFE) {
      return m_owner->setState<PlayerShoot>(context, player);
    } else {
      return m_owner->setState<PlayerAttack>(context, player);
    }
  }

  if(isButtonPressed(FRMouseButton::RIGHT)) {
    return m_owner->setState<PlayerAttack>(context, player);
  }

}

void PlayerMove::onEnter(ECSContext& context, Entity player) {
  Model* model = context.registry->getComponent<Model>(player, ComponentID::Model);
  Player* playerComponent = context.registry->getComponent<Player>(player, ComponentID::Player);

  WeaponType currentWeapon = playerComponent->weapons[playerComponent->currentWeaponIndex].type;
  if(currentWeapon == WeaponType::KNIFE) {
    setAnimation(model->sprite, "knife_move");
  }
  else if(currentWeapon == WeaponType::PISTOL) {
    setAnimation(model->sprite, "pistol_move");
  }
  else if(currentWeapon == WeaponType::SHOTGUN) {
    setAnimation(model->sprite, "shotgun_move");
  }
  else if(currentWeapon == WeaponType::RIFLE) {
    setAnimation(model->sprite, "rifle_move");
  }
}

void PlayerMove::update(ECSContext& context, Entity player, real deltaTime) {
  Model* model = context.registry->getComponent<Model>(player, ComponentID::Model);
  Player* playerComponent = context.registry->getComponent<Player>(player, ComponentID::Player);
  Physics* physics = context.registry->getComponent<Physics>(player, ComponentID::Physics);

  updateAnimation(model->sprite, deltaTime);

  if(physics->transition && physics->idling) {
    return m_owner->setState<PlayerIdle>(context, player);
  }

  WeaponType currentWeapon = playerComponent->weapons[playerComponent->currentWeaponIndex].type;

  if(isButtonPressed(FRMouseButton::LEFT)) {
    if(currentWeapon != WeaponType::KNIFE) {
      return m_owner->setState<PlayerShoot>(context, player);
    } else {
      return m_owner->setState<PlayerAttack>(context, player);
    }
  }

  if(isButtonPressed(FRMouseButton::RIGHT)) {
    return m_owner->setState<PlayerAttack>(context, player);
  }

}

void PlayerShoot::onEnter(ECSContext& context, Entity player) {
  Model* model = context.registry->getComponent<Model>(player, ComponentID::Model);
  Player* playerComponent = context.registry->getComponent<Player>(player, ComponentID::Player);

  WeaponData currentWeapon = playerComponent->weapons[playerComponent->currentWeaponIndex];

  if(currentWeapon.type == WeaponType::KNIFE || !hasAvailableAmmo(playerComponent)) {
    return m_owner->setState<PlayerIdle>(context, player);
  }

  if(needToReload(playerComponent)) {
    return m_owner->setState<PlayerReload>(context, player);
  }

  if(currentWeapon.type == WeaponType::PISTOL) {
    setAnimation(model->sprite, "pistol_shoot");
  }
  else if(currentWeapon.type == WeaponType::SHOTGUN) {
    setAnimation(model->sprite, "shotgun_shoot");
  }
  else if(currentWeapon.type == WeaponType::RIFLE) {
    setAnimation(model->sprite, "rifle_shoot");
  }
}

void PlayerShoot::update(ECSContext& context, Entity player, real deltaTime) {

  Model* model = context.registry->getComponent<Model>(player, ComponentID::Model);
  Player* playerComponent = context.registry->getComponent<Player>(player, ComponentID::Player);
  Physics* physics = context.registry->getComponent<Physics>(player, ComponentID::Physics);

  updateAnimation(model->sprite, deltaTime);
  if(isAnimationFinished(model->sprite)) {

    // TODO(mizofix): generate bullet based on weapon type

    Transformation* transf = context.registry->getComponent<Transformation>(player,
                                                                            ComponentID::Transformation);

    vec2 offset = playerComponent->weapons[playerComponent->currentWeaponIndex].handOffset;
    vec2 playerHeading = degToVec(transf->angle);
    vec2 playerSide = playerHeading.perp();

    vec2 bulletPosition = playerHeading * offset.x + playerSide * offset.y + transf->position;

    Bullet bullet = generateBullet(context.registry, playerHeading, bulletPosition,
                                   playerComponent->weapons[playerComponent->currentWeaponIndex]);

    vec2 explosionPosition = bulletPosition + playerHeading * 11.0f;
    generateExplosion(explosionPosition, transf->angle);
    // /////////////////////

    playerComponent->weapons[playerComponent->currentWeaponIndex].ammo--;
    if(needToReload(playerComponent)) {
      return m_owner->setState<PlayerReload>(context, player);
    }

    if(isButtonPressed(FRMouseButton::LEFT) && hasAvailableAmmo(playerComponent)) {
      resetAnimation(model->sprite);
    } else {

      if(physics->idling) {
        return m_owner->setState<PlayerIdle>(context, player);
      } else {
        return m_owner->setState<PlayerMove>(context, player);
      }

    }
  }

}
Entity PlayerShoot::generateBullet(Registry* registry,
                                   const vec2& direction, const vec2& position,
                                   const WeaponData& data) {

  Entity bullet = registry->createEntity();
  Physics* physics = new Physics();
  physics->velocity = direction * data.speed;
  physics->size = data.bulletSize;

  Transformation* transformation = new Transformation();
  transformation->position = position;
  transformation->angle = vecToDeg(direction);

  Bullet* bulletComponent = new Bullet();
  bulletComponent->lifetime = data.lifetime;
  bulletComponent->durability = data.durability;
  bulletComponent->damage = data.damage;

  registry->addComponent(bullet, physics);
  registry->addComponent(bullet, transformation);
  registry->addComponent(bullet, bulletComponent);

  Entity trail = registry->createEntity();
  Trail* trailComp = new Trail(bullet,
                               data.trailLifetime,
                               data.trailMaxAngle,
                               data.trailScatterSpeed,
                               data.bulletSize);

  registry->addComponent(trail, trailComp);

  return bullet;
}

void PlayerShoot::generateExplosion(const vec2& position, real angle) {
  Message msg;
  msg.type = int(MessageType::SPAWN_EFFECT);
  msg.effect_info.type = EffectType::GUN_EXPLOSION;
  msg.effect_info.x = position.x;
  msg.effect_info.y = position.y;
  msg.effect_info.scale = 1.0f;
  msg.effect_info.angle = angle;
  msg.effect_info.lifetime = 5.0f;
  msg.effect_info.fadeOut = false;

  notify(msg);
}

bool PlayerShoot::needToReload(Player* player) {
  return (player->weapons[player->currentWeaponIndex].ammo <= 0 &&
    player->weapons[player->currentWeaponIndex].availableClips > 0);
}

bool PlayerShoot::hasAvailableAmmo(Player* player) {
  return (player->weapons[player->currentWeaponIndex].ammo > 0 ||
          player->weapons[player->currentWeaponIndex].availableClips > 0);

}

void PlayerAttack::onEnter(ECSContext& context, Entity player) {
  Model* model = context.registry->getComponent<Model>(player, ComponentID::Model);
  Player* playerComponent = context.registry->getComponent<Player>(player, ComponentID::Player);

  WeaponData currentWeapon = playerComponent->weapons[playerComponent->currentWeaponIndex];

  if(currentWeapon.type == WeaponType::KNIFE) {
    setAnimation(model->sprite, "knife_attack");
  }
  else if(currentWeapon.type == WeaponType::PISTOL) {
    setAnimation(model->sprite, "pistol_attack");
  }
  else if(currentWeapon.type == WeaponType::SHOTGUN) {
    setAnimation(model->sprite, "shotgun_attack");
  }
  else if(currentWeapon.type == WeaponType::RIFLE) {
    setAnimation(model->sprite, "rifle_attack");
  }
}

void PlayerAttack::update(ECSContext& context, Entity player, real deltaTime) {

  Model* model = context.registry->getComponent<Model>(player, ComponentID::Model);
  Player* playerComponent = context.registry->getComponent<Player>(player, ComponentID::Player);
  Physics* physics = context.registry->getComponent<Physics>(player, ComponentID::Physics);

  updateAnimation(model->sprite, deltaTime);
  if(isAnimationFinished(model->sprite)) {

    if(isButtonPressed(FRMouseButton::LEFT)) {
      resetAnimation(model->sprite);
    } else {

      if(physics->idling) {
        return m_owner->setState<PlayerIdle>(context, player);
      } else {
        return m_owner->setState<PlayerMove>(context, player);
      }

    }
  }

}


void PlayerReload::onEnter(ECSContext& context, Entity player) {
  Model* model = context.registry->getComponent<Model>(player, ComponentID::Model);
  Player* playerComponent = context.registry->getComponent<Player>(player, ComponentID::Player);

  WeaponData currentWeapon = playerComponent->weapons[playerComponent->currentWeaponIndex];

  if(currentWeapon.type == WeaponType::KNIFE || currentWeapon.availableClips < 0) {
    return m_owner->setState<PlayerIdle>(context, player);
  }
  else if(currentWeapon.type == WeaponType::PISTOL) {
    setAnimation(model->sprite, "pistol_reload");
  }
  else if(currentWeapon.type == WeaponType::SHOTGUN) {
    setAnimation(model->sprite, "shotgun_reload");
  }
  else if(currentWeapon.type == WeaponType::RIFLE) {
    setAnimation(model->sprite, "rifle_reload");
  }
}

void PlayerReload::update(ECSContext& context, Entity player, real deltaTime) {

  Model* model = context.registry->getComponent<Model>(player, ComponentID::Model);
  Player* playerComponent = context.registry->getComponent<Player>(player, ComponentID::Player);
  Physics* physics = context.registry->getComponent<Physics>(player, ComponentID::Physics);

  updateAnimation(model->sprite, deltaTime);
  if(isAnimationFinished(model->sprite)) {

    playerComponent->weapons[playerComponent->currentWeaponIndex].ammo = playerComponent->weapons[playerComponent->currentWeaponIndex].clipSize;
    playerComponent->weapons[playerComponent->currentWeaponIndex].availableClips--;

    if(isButtonPressed(FRMouseButton::LEFT)) {
      return m_owner->setState<PlayerShoot>(context, player);
    } else {
      if(physics->idling) {
        return m_owner->setState<PlayerIdle>(context, player);
      } else {
        return m_owner->setState<PlayerMove>(context, player);
      }
    }

    return;
  }

}
