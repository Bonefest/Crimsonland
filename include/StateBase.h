#ifndef STATE_BASE_H_INCLUDED
#define STATE_BASE_H_INCLUDED

class StateController;

#include "ecs/System.h"
#include "StateController.h"

class StateBase {
 public:

  virtual ~StateBase() { }

  virtual void onEnter(ECSContext& context, Entity target) { }
  virtual void update(ECSContext& context, Entity target, real deltaTime) { }
  virtual void onExit(ECSContext& context, Entity target) { }

  void setOwner(StateController* owner);

protected:
  StateController* m_owner;
};

#endif
