
#include "PlayerStates.h"
#include "Framework.h"

void PlayerStateBase::setOwner(PlayerStateController* owner) {
    m_owner = owner;
}
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
    m_owner->setState(context, player, PlayerState::Move);
    return;
  }

  WeaponType currentWeapon = playerComponent->weapons[playerComponent->currentWeaponIndex].type;

  if(isButtonPressed(FRMouseButton::LEFT)) {
    if(currentWeapon != WeaponType::KNIFE) {
      m_owner->setState(context, player, PlayerState::Shoot);
    } else {
      m_owner->setState(context, player, PlayerState::MeleeAttack);
    }

    return;
  }

  if(isButtonPressed(FRMouseButton::RIGHT)) {
    m_owner->setState(context, player, PlayerState::MeleeAttack);
    return;
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
    m_owner->setState(context, player, PlayerState::Idle);
    return;
  }

  WeaponType currentWeapon = playerComponent->weapons[playerComponent->currentWeaponIndex].type;

  if(isButtonPressed(FRMouseButton::LEFT)) {
    if(currentWeapon != WeaponType::KNIFE) {
      m_owner->setState(context, player, PlayerState::Shoot);
    } else {
      m_owner->setState(context, player, PlayerState::MeleeAttack);
    }

    return;
  }

  if(isButtonPressed(FRMouseButton::RIGHT)) {
    m_owner->setState(context, player, PlayerState::MeleeAttack);
    return;
  }

}

void PlayerShoot::onEnter(ECSContext& context, Entity player) {
  Model* model = context.registry->getComponent<Model>(player, ComponentID::Model);
  Player* playerComponent = context.registry->getComponent<Player>(player, ComponentID::Player);

  WeaponData currentWeapon = playerComponent->weapons[playerComponent->currentWeaponIndex];

  if(currentWeapon.type == WeaponType::KNIFE) {
    m_owner->setState(context, player, PlayerState::Idle);
  }

  if(!hasAmmo(playerComponent)) {
    m_owner->setState(context, player, PlayerState::Reload);
    return;
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

    playerComponent->weapons[playerComponent->currentWeaponIndex].ammo--;
    if(!hasAmmo(playerComponent)) {
      m_owner->setState(context, player, PlayerState::Reload);
      return;
    }

    if(isButtonPressed(FRMouseButton::LEFT)) {
      resetAnimation(model->sprite);
    } else {

      if(physics->idling) {
        m_owner->setState(context, player, PlayerState::Idle);
      } else {
        m_owner->setState(context, player, PlayerState::Move);
      }

      return;
    }
  }

}
bool PlayerShoot::hasAmmo(Player* player) {
  return player->weapons[player->currentWeaponIndex].ammo >= 0;
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
        m_owner->setState(context, player, PlayerState::Idle);
      } else {
        m_owner->setState(context, player, PlayerState::Move);
      }

      return;
    }
  }

}
