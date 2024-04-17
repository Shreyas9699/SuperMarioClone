#pragma once
#include "Vec2.h"
#include "Animation.h"
#include <SFML/Graphics.hpp>

class Components
{
public:
	bool has = false;
};

class CTransform : public Components
{
public:
	Vec2 pos	  = { 0.0, 0.0 };
	Vec2 prevPos  = { 0.0, 0.0 };
	Vec2 scale	  = { 1.0, 1.0 };
	Vec2 velocity = { 0.0, 0.0 };
	float angle	 = 0;

	CTransform() {}
	CTransform(const Vec2& p)
		: pos(p), prevPos(p) {}
	CTransform(const Vec2& p, const Vec2& sp, const Vec2& sc, float a)
		: pos(p), prevPos(p), velocity(sp), scale(sc), angle(a) {}
};

class CBoundingBox : public Components
{
public:
	Vec2 size;
	Vec2 halfSize;

	CBoundingBox() {}
	CBoundingBox(const Vec2& siz)
		: size(siz), halfSize(siz.x / 2, siz.y/ 2){}
};

class CLifeSpan : public Components
{
public:
	sf::Clock clock{};
	float lifeSpan = 0;

	CLifeSpan() {}
	explicit CLifeSpan(float l) 
		: lifeSpan(l) 
	{ 
		clock.restart();
	}
	// int lifeSpan		= 0;
	// int frameCreated	= 0;

	// CLifeSpan() {}
	// CLifeSpan(int l, int f)
	//	: lifeSpan(l), frameCreated(f) {}
};

class CInput : public Components
{
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool shoot = false;
	bool canShoot = true;
	bool canJump = true;

	CInput() {}
};

class CAnimation : public Components
{
public:
	Animation animation;
	bool repeat = false;

	CAnimation() {}
	CAnimation(const Animation& a, bool r)
		: animation(a), repeat(r) {}
};

class CGravity : public Components
{
public:
	float gravity = 0.0f;

	CGravity() {}
	CGravity(float g)
		: gravity(g) {}
};

class CState : public Components
{
public:
	std::string state = "Air";

	CState() {}
	CState(const std::string& s)
		: state(s) {}
};