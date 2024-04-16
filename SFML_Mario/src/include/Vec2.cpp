#include "Vec2.h"
#include <math.h>

constexpr auto PI = 3.14159265;

Vec2::Vec2() {}

Vec2::Vec2(float xin, float yin)
	: x(xin), y(yin) {}

bool Vec2::operator == (const Vec2& rhs) const
{
    return (x == rhs.x) && (y == rhs.y);
}

bool Vec2::operator != (const Vec2& rhs) const
{
    return (x != rhs.x) && (y != rhs.y);
}

Vec2 Vec2::operator + (const Vec2& rhs) const 
{
    return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
    return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator / (const float val) const
{ 
    return Vec2(x / val, y / val);
}

Vec2 Vec2::operator * (const float val) const
{
    return Vec2(x * val, y * val);
}

Vec2 Vec2::operator += (const Vec2& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
}

Vec2 Vec2::operator -= (const Vec2& rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    return *this;
}

Vec2 Vec2::operator /= (const float val)
{
    this->x /= val;
    this->y /= val;
    return *this;
}

Vec2 Vec2::operator *= (const float val)
{
    this->x *= val;
    this->y *= val;
    return *this;
}

float  Vec2::dist(const Vec2& rhs) const
{
    // Using Pythagorus Dist = sqrtf(D.x*D.x + D.y*D.y) 
    float dx = x - rhs.x;
    float dy = y - rhs.y;
    float sq_dist = dx * dx + dy * dy;
    return std::sqrtf(sq_dist);
}

std::ostream& operator << (std::ostream& stream, const Vec2& vector)
{ 
    return stream << vector.x << " " << vector.y << "\n"; 
}