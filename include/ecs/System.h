#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include "Common.h"

class Registry;

struct ECSContext {
  Registry* registry;
  WorldData data;
};

#include "ecs/Registry.h"

class System {
public:
  virtual ~System() { }

  virtual bool init(ECSContext& context) { return true; }
  virtual void update(ECSContext& context, real deltaTime) { }
  virtual void draw(ECSContext& context) { }
};

#endif
