#pragma once

#include "Vec2.h"
#include "Graphics.h"
#include "LineSegment.h"
#include "Geometry.h"
#include "Ray.h"
#include <vector>
#include <algorithm>


struct LightPoint {
	LightPoint(Vei2 loc, float angle)
		:
		loc(loc),
		angle(angle)
	{
	}
	void SetAngle(float alpha)
	{
		angle = alpha;
	}
	Vei2 loc;
	float angle;
};

class Light
{
public:
	Light() = default;
public:
	void AddLightsource(const Vei2& lightsource);
	Vei2 RayCast(std::vector<LineSegment> wallVertices, Vec2 direction, Vei2 toPoint, const Vei2& lightSource);
	std::vector<LightPoint> LineOfSight(std::vector<LineSegment> wallVertices,const Vei2& lightSource);
	void Draw(std::vector<LineSegment> wallVertices, Graphics& gfx);
	std::vector<Vei2> lightSources;
private:
	Vei2 Invalid = { 12345678, 12345678 };
};
