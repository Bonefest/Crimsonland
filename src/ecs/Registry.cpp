
#include "ecs/Registry.h"
#include "Message.h"
#include "Assert.h"

Registry::Registry(): m_newEntityID(0) { }

Registry::~Registry() {
  clear();
}

Entity Registry::createEntity(Bitset components) {
  Entity newEntity = m_newEntityID++;
  m_entities[newEntity] = Components();
  m_entities[newEntity].second.fill(nullptr);

  for(ComponentID component = ComponentID(0);
      component < ComponentID::COUNT;
      component = ComponentID(int(component) + 1)) {

    if(components.isSet(int(component))) {
      m_entities[newEntity].second[int(component)] = componentFactory(component);
      if(m_entities[newEntity].second[int(component)] != nullptr) {
        m_entities[newEntity].first.setBit(int(component ));
      }
    }
  }

  Message msg(int(MessageType::ECS_ENTITY_CREATED));
  msg.entity_info.entity = newEntity;
  notify(msg);

  return newEntity;
}

void Registry::destroyEntity(Entity entity) {
  auto entityIt = m_entities.find(entity);
  if(entityIt != m_entities.end()) {
    Message msg(int(MessageType::ECS_ENTITY_DELETE));
    msg.entity_info.entity = entity;
    msg.entity_info.registry = this;
    notify(msg);

    for(Component* component : entityIt->second.second) {
      if(component != nullptr) {
        delete component;
      }
    }

    m_entities.erase(entityIt);
  }
}

bool Registry::isEntityExists(Entity entity) const {
  return m_entities.find(entity) != m_entities.end();
}

void Registry::addComponent(Entity entity, ComponentID component) {
  auto entityIt = m_entities.find(entity);
  if(entityIt != m_entities.end()) {
    if(!entityIt->second.first.isSet(int(component))) {
      Component* newComponent= componentFactory(component);
      if(newComponent != nullptr) {
        entityIt->second.second[int(component)] = newComponent;
        entityIt->second.first.setBit(int(component));
      }
    }
  }
}

void Registry::addComponent(Entity entity, Component* component) {
  Assert(component != nullptr);

  auto entityIt = m_entities.find(entity);
  if(entityIt != m_entities.end()) {
    ComponentID id = component->getID();

    if(!entityIt->second.first.isSet(int(id))) {
      entityIt->second.second[int(id)] = component;
      entityIt->second.first.setBit(int(id));
    }
  }
}

void Registry::removeComponent(Entity entity, ComponentID id) {
  auto entityIt = m_entities.find(entity);
  if(entityIt != m_entities.end()) {
    if(entityIt->second.first.isSet(int(id))) {
      Assert(entityIt->second.second[int(id)] != nullptr);

      entityIt->second.first.setBit(int(id));
      delete entityIt->second.second[int(id)];
    }
  }
}

bool Registry::hasComponent(Entity entity, ComponentID id) {
  auto entityIt = m_entities.find(entity);
  if(entityIt == m_entities.end()) {
    return false;
  }

  return entityIt->second.first.isSet(int(id));
}

EntitiesContainer Registry::findEntities(Entity entity, Bitfield components) {
  EntitiesContainer result;
  for(auto entityPair: m_entities) {
    if(entityPair.second.first.isSetBits(components)) {
      result.push_back(entityPair.first);
    }
  }

  return result;
}

void Registry::clear() {
  for(auto entityPair : m_entities) {
    for(auto component : entityPair.second.second) {
      if(component != nullptr) {
        delete component;
      }
    }
  }

  m_newEntityID = 0;
  m_entities.clear();
}
