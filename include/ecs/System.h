#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include "Common.h"
#include "ecs/Registry.h"

struct ECSContext {
  Registry* registry;
  WorldData data;
};

class System {
public:
  virtual void init(ECSContext context) { }
  virtual void update(ECSContext context, real deltaTime) { }
  virtual void draw(ECSContext context) { }
};

class RenderingSystem: public System {
public:
  virtual void update(ECSContext context, real deltaTime) {

  }
}

#endif
