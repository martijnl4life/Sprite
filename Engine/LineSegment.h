#pragma once
#include "Vec2.h"
#include "Graphics.h"
#include <vector>

struct LineSegment
{
public:
	LineSegment() = default;
	LineSegment(const Vei2 p0, const Vei2 p1)
		:
		p0(p0),
		p1(p1)
	{
	}
public:
	Vei2 p0;
	Vei2 p1;
};