#ifndef BASE_H_INCLUDED
#define BASE_H_INCLUDED

#include "SDL2/SDL.h"
#include "Framework.h"
#include "Program.h"
#include "Common.h"
#include "Assert.h"

#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <cstdio>

#include "Math.h"

class Object {
public:

  void setPosition(const vec2& position) {
    m_position = position;
  }

  void setPosition(real x, real y) {
    m_position.x= x;
    m_position.y = y;
  }

  // NOTE(mizofix): We use circle shape for physics calculations
  // but it could be easily moved to AABB
  void setSize(real size) {
    m_size = size;
  }

  void setAngle(real angle) {
    m_angle = angle;
  }
  void setMaxSpeed(real speed) {
    m_maxSpeed = speed;
  }

  void setAcceleration(const vec2& acceleration) {
    m_acceleration = acceleration;
  }

  void setVelocity(const vec2& velocity) {
    m_velocity = velocity;
  }

  virtual void draw(bool relativeToCamera = true) {
    drawSprite(m_sprite, int(round(m_position.x)), int(round(m_position.y)), m_angle, relativeToCamera);
  }

  virtual void update(real deltaTime) {
    m_velocity += m_acceleration * deltaTime;
    m_position += m_velocity * deltaTime;
  }

protected:
  Sprite* m_sprite;
  real m_size;

  vec2 m_position;
  vec2 m_acceleration;
  vec2 m_velocity;

  real m_maxSpeed;
  real m_angle;
};

class Entity: public Object {
public:

  void setMaxHealth(real maxHealth) {
    m_maxHealth = maxHealth;
  }

  void setHealth(real health) {
    m_health = health;
  }

protected:
  real m_maxHealth;
  real m_health;

};

class Zombie : public Entity {

};

class ZombieController {
public:
  virtual void controll(Zombie* zombie) = 0;
};

class AggressiveZombie: public ZombieController {
  void controll(Zombie* zombie);
};


class CrimsonlandFramework : public Framework {

public:

  CrimsonlandFramework(int argc, char** commands);

  virtual void PreInit(int& width, int& height, bool& fullscreen);

  virtual bool Init();

  virtual void Close();

  virtual bool Tick();

  virtual void onMouseMove(int x, int y, int xrelative, int yrelative);

  virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) { }

  virtual void onKeyPressed(FRKey k);

  virtual void onKeyReleased(FRKey k) { }

private:

  void update();
  void draw();
  void drawToScreen();
  void updateTimer();

  WorldData m_worldData;

  Texture* m_screenTexture;
  Sprite* m_testSprite;

  float m_spritePosX;
  float m_spritePosY;

  Program m_bumpProgram;

  float m_lastTime;
  float m_deltaTime;

  float m_angle;
};


#endif
