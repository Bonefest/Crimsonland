#ifndef SYSTEM_MANAGER_H_INCLUDED
#define SYSTEM_MANAGER_H_INCLUDED

#include "ecs/System.h"

#include <list>

class SystemManager {
public:

  ~SystemManager() {
    for(auto system: m_systems) {
      delete system;
    }
  }

  void addSystem(ECSContext context, System* system) {
    m_systems.push_back(system);
    system->init(context);
  }

  void updateSystems(ECSContext context, real deltaTime) {
    for(auto system: m_systems) {
      system->update(context, deltaTime);
    }
  }

  void drawSystems(ECSContext context) {
    for(auto system: m_systems) {
      system->draw(context);
    }
  }

private:
  std::list<System*> m_systems;
};

#endif
