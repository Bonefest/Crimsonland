
#ifndef REGISTRY_H_INCLUDED
#define REGISTRY_H_INCLUDED

#include "Common.h"
#include "ecs/Bitset.h"
#include "ecs/Components.h"

#include <list>
#include <array>
#include <unordered_map>

using Components = std::pair<Bitset, std::array<Component*, int(ComponentID::COUNT)>>;
using Entities = std::unordered_map<Entity, Components>;
using EntitiesContainer = std::list<Entity>;

template <typename Component>
Bitfield buildBitfield(Component comp) {
  return (1 << int(comp));
}

// NOTE(mizofix): takes components, and builds based on it a single Bitfield
template <typename Component, typename ... Components>
Bitfield buildBitfield(Component comp, Components... comps) {
  return (1 << int(comp)) | buildBitfield(comps...);
}

// NOTE(mizofix): Registry is responsible for storing everything about an entity,
// managing it's lifetime etc. Even though components are created behind this place,
// user should not delete them herself.
class Registry {
public:

  Registry();
  ~Registry();

  Entity createEntity(Bitset components = Bitset());
  void destroyEntity(Entity entity);
  bool isEntityExists(Entity entity) const;
  // NOTE(mizofix): creates new component, based on its id
  void addComponent(Entity entity, ComponentID component);
  // NOTE(mizofix): simply adds a new component, created somewhere
  // else
  void addComponent(Entity entity, Component* component);

  void removeComponent(Entity entity, ComponentID id);
  bool hasComponent(Entity entity, ComponentID id);

  EntitiesContainer findEntities(Bitfield components);

  // NOTE(mizofix): shouldn't we move getID() to static?
  template <typename T>
  T* getComponent(Entity entity, ComponentID id) {

    auto entityIt = m_entities.find(entity);
    if(entityIt == m_entities.end()) {
      return nullptr;
    }

    if(!entityIt->second.first.isSet(int(id))) {
      return nullptr;
    }

    return static_cast<T*>(entityIt->second.second[int(id)]);
  }

private:
  void clear();

  uint32_t m_newEntityID;
  Entities m_entities;

};

#endif
