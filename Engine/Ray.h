#pragma once
#include "Vec2.h"
#include "Graphics.h"
#include <vector>

struct Ray
{
public:
	Ray() = default;
	Ray(const Vec2 p0, const Vec2 p1)
		:
		p0(p0),
		p1(p1)
	{
	}
public:
	Vec2 p0;
	Vec2 p1;
};