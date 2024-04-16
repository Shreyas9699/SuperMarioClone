#pragma once
#include <iostream>

// defines a C++ class Vec2 representing a 2D vector with x and y components
// along with overloaded arithmetic operators for addition, subtraction
// and scalar multiplication. Additionally, it includes member functions for scaling and adding values to the vector

class Vec2
{
public:
	float x = 0;
	float y = 0;

    Vec2();
	Vec2(float xin, float yin);

    bool operator == (const Vec2& rhs) const;
    bool operator != (const Vec2& rhs) const;

    Vec2 operator + (const Vec2& rhs) const;
    Vec2 operator - (const Vec2& rhs) const;
    Vec2 operator * (const Vec2& rhs) const;
    Vec2 operator / (const float val) const;
    Vec2 operator * (const float val) const;

    Vec2 operator += (const Vec2& rhs);
    Vec2 operator -= (const Vec2& rhs);
    Vec2 operator /= (const float val);
    Vec2 operator *= (const float val);

    float dist(const Vec2& rhs) const;

    friend std::ostream& operator << (std::ostream& stream, const Vec2& vector);
};