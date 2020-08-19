
#include "Math.h"
#include <cmath>

vec2::vec2():x(0.0f), y(0.0f) { }
vec2::vec2(real inX, real inY): x(inX), y(inY) { }

vec2 vec2::operator+(const vec2& vec) const {
  return vec2(x + vec.x, y + vec.y);
}

vec2& vec2::operator+=(const vec2& vec) {
  x += vec.x;
  y += vec.y;
  return *this;
}

vec2 vec2::operator-(const vec2& vec) const {
  return vec2(x - vec.x, y - vec.y);
}

vec2& vec2::operator-=(const vec2& vec) {
  x -= vec.x;
  y -= vec.y;
  return *this;
}

vec2 vec2::operator*(real value) const {
  return vec2(x * value, y * value);
}

vec2 operator*(real value, const vec2& vec) {
  return vec * value;
}

vec2& vec2::operator*=(real value) {
  x *= value;
  y *= value;
  return *this;
}

real vec2::dot(const vec2& vec) const {
  return x * vec.x + y * vec.y;
}

real vec2::length() const {
  return sqrt(x*x + y*y);
}

real vec2::distance(const vec2& vec) const {
  return (*this - vec).length();
}


void vec2::normalize() {
  real len = length();
  if(len > 0.01) {
    x /= len;
    y /= len;
  }
}

vec2 vec2::perp() const {
  return vec2(-y, x);
}


real degToRad(real deg) {
  return (deg / 180.0f) * PI;
}

real radToDeg(real rad) {
  return (rad / PI) * 180.0f;
}

vec2 radToVec(real rad) {
  return vec2(cos(rad), sin(rad));
}

real vecToRad(const vec2& vec) {
  return atan2(vec.y, vec.x);
}
