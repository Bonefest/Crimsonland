#include "Message.h"
#include <list>
#include <unordered_map>

static std::list<Message> g_messageContainer;
static std::unordered_map<int, std::list<MessageFunction>> g_subscribers;

void addNewMessage(Message message) {
  g_messageContainer.push_back(message);


}

bool pollMessage(Message& message) {
  if(peekMessage(message)) {
    g_messageContainer.pop_front();

    return true;
  }

  return false;
}

bool peekMessage(Message& message) {
  if(!g_messageContainer.empty()) {
    message = g_messageContainer.front();

    return true;
  }

  return false;
}

bool hasMessage() {
  return !g_messageContainer.empty();
}

void subscribeToMessage(int message, MessageFunction function) {
  g_subscribers[message].push_back(function);
}

void notify(Message message) {
  auto subscribersIt = g_subscribers.find(message.type);
  if(subscribersIt != g_subscribers.end()) {
    for(auto subscriber: subscribersIt->second) {
      subscriber(message);
    }
  }
}

void clearSubscribers() {
  g_subscribers.clear();
}
