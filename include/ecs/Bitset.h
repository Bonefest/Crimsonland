
#ifndef BITSET_H_INCLUDED
#define BITSET_H_INCLUDED

#include <stdint.h>

using Bitfield = uint32_t;

class Bitset {
public:

  Bitset();
  Bitset(Bitfield bits);

  void setBits(Bitfield bits, bool reset = false);
  void setBit(Bitfield bit);

  bool isSet(Bitfield bit) const;
  bool isSetBits(Bitfield bits) const;

  void clear();

 private:
  Bitfield m_bits;
};

#endif
