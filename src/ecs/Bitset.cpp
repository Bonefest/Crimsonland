#include "ecs/Bitset.h"

Bitset::Bitset(): m_bits(0) { }
Bitset::Bitset(Bitfield bits): m_bits(bits) { }

void Bitset::setBits(Bitfield bits, bool reset) {
  if(reset) {
    m_bits = bits;
  } else {
    m_bits |= bits;
  }
}

void Bitset::setBit(Bitfield bit) {
  m_bits |= 1 << bit;
}

bool Bitset::isSet(Bitfield bit) const {
  return (1 << bit & m_bits);
}

bool Bitset::isSetBits(Bitfield bits) const {
  return m_bits & bits;
}

void Bitset::clear() {
  m_bits = 0;
}
