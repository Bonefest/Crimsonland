#ifndef FAMILY_H_INCLUDED
#define FAMILY_H_INCLUDED

// NOTE(mizofix): generates a unique ID for each type
class Family {
public:
  template <class>
  static std::size_t id() {
    static const std::size_t _id = generateNewID();
    return _id;
  }
private:
  static std::size_t generateNewID() {
    static std::size_t _newID = 0;
    return _newID++;
  }

};

#endif
