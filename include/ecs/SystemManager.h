#ifndef SYSTEM_MANAGER_H_INCLUDED
#define SYSTEM_MANAGER_H_INCLUDED

#include "ecs/System.h"

#include <list>
#include <string>
#include <unordered_map>

class SystemManager {
public:

  ~SystemManager() {
    clear();
  }

  bool addSystem(ECSContext& context, System* system, const std::string& name) {
    if(!system->init(context)) {
      delete system;
      return false;
    }

    Assert(m_systemPairs.emplace(name, system).second);
    m_systems.push_back(system);

    return true;
  }

  void removeSystem(const std::string& name) {
    auto systemIt = m_systemPairs.find(name);
    if(systemIt != m_systemPairs.end()) {
      m_systems.remove(systemIt->second);
      delete systemIt->second;
      m_systemPairs.erase(systemIt);
    }
  }

  void updateSystems(ECSContext& context, real deltaTime) {
    for(auto system: m_systems) {
      system->update(context, deltaTime);
    }
  }

  void drawSystems(ECSContext& context) {
    for(auto system: m_systems) {
      system->draw(context);
    }
  }

  void clear() {
    for(auto system: m_systems) {
      delete system;
    }

    m_systemPairs.clear();
    m_systems.clear();
  }

private:

  std::unordered_map<std::string, System*> m_systemPairs;
  std::list<System*> m_systems;

};

#endif
