#ifndef STATE_CONTROLLER_H_INCLUDED
#define STATE_CONTROLLER_H_INCLUDED

class StateBase;

#include "StateBase.h"
#include "Family.h"

#include <unordered_map>

//class StateBase;

class StateController {
public:
  StateController();
  ~StateController();

  void init(ECSContext& context, Entity target);
  void update(ECSContext& context, Entity target, real deltaTime);

  template <class State>
  void setState(ECSContext& context, Entity target) {
    if(m_currentState != nullptr) {
      m_currentState->onExit(context, target);
    }

    auto stateIt = m_states.find(Family::id<State>());
    if(stateIt == m_states.end()) {
      m_currentState = new State();
      m_currentState->setOwner(this);
      m_states[Family::id<State>()] = m_currentState;
    } else {
      m_currentState = stateIt->second;
    }

    m_currentState->onEnter(context, target);
  }

private:
  std::unordered_map<std::size_t, StateBase*> m_states;
  StateBase* m_currentState;
};



#endif
