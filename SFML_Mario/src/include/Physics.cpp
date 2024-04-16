#include "Physics.h"

static Vec2 overlap(const Vec2& p1, const Vec2& p2, const Vec2& s1, const Vec2& s2)
{
	/*
		AABB intersections:
		dx = abs(x2 - x1), w1 / 2 and w2 / 2
		so, if (w1 / 2) + (w2 / 2) - dx > 0, then we have a x overlap

		dy = abs(y2 - y1), h1 / 2 and h2 / 2
		so, if (h1 / 2) + (h2 / 2) - dy > 0, then we have a y overlap

		delat = [abs(x2 - x1), abs(y2 - y1)]
		ox = (w1 / 2) + (w2 / 2) - delat.x
		oy = (h1 / 2) + (h2 / 2) - delat.y
		overlpa = [ox, oy]
	*/
	float overlapX = s2.x + s1.x - abs(p2.x - p1.x);
	float overlapY = s2.y + s1.y - abs(p2.y - p1.y);

	return Vec2(overlapX, overlapY);
}

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> eA, std::shared_ptr<Entity> eB)
{
	// return the overlap rectangle size of the bounding boxes of entity eA and eB
	// Create instances of CTransform and CBoundingBox using entity pointers
	const auto& p1 = eA->getComponent<CTransform>().pos;
	const auto& p2 = eB->getComponent<CTransform>().pos;
	const auto& bb1 = eA->getComponent<CBoundingBox>().halfSize;
	const auto& bb2 = eB->getComponent<CBoundingBox>().halfSize;

	// Return the overlap rectangle size of the bounding boxes of entity eA and eB
	return overlap(p1, p2, bb1, bb2);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> eA, std::shared_ptr<Entity> eB)
{
	// return the previous overlap rectangle size of the bounding boxes of entity eA and eB
	// Note that this uses entity's previous position
	const auto& p1 = eA->getComponent<CTransform>().prevPos;
	const auto& p2 = eB->getComponent<CTransform>().prevPos;
	const auto& bb1 = eA->getComponent<CBoundingBox>().halfSize;
	const auto& bb2 = eB->getComponent<CBoundingBox>().halfSize;

	return overlap(p1, p2, bb1, bb2);
}