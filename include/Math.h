#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED

#define PI 3.1415926
using real = float;

struct vec2 {

  vec2();
  vec2(real inX, real inY);

  vec2 operator+(const vec2& vec) const;
  vec2& operator+=(const vec2& vec);
  vec2 operator-(const vec2& vec) const;
  vec2& operator-=(const vec2& vec);
  vec2 operator*(real value) const;
  vec2& operator*=(real value);

  real dot(const vec2& vec) const;
  real length() const;
  real sqLength() const;
  real distance(const vec2& vec) const;
  void normalize();
  vec2 perp() const;

  friend vec2 operator*(real value, const vec2& vec);

  union {

    real coords[2];

    struct {
      real x;
      real y;
    };

  };

};

real degToRad(real deg);
real radToDeg(real rad);
vec2 radToVec(real rad);
vec2 degToVec(real deg);

real vecToRad(const vec2& vec);
real vecToDeg(const vec2& vec);
#endif
