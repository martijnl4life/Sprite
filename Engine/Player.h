#pragma once

#include "Vec2.h"
#include "Graphics.h"
#include "LineSegment.h"
#include "Keyboard.h"
#include "Geometry.h"
#include <utility>

class Player
{
public:
	Player(Vec2& pos, Vec2& dir = Vec2(1, 0));
	void Update(float dt, const Keyboard& kbd);
	Vei2 raycasted(std::vector<LineSegment> wallVertices, Vec2 direction, Vei2 toPoint);
	std::vector<Vei2> LineOfSight(std::vector<LineSegment> wallVertices);
	Vec2 GetPos() const { return pos; }
	void Draw(Graphics& gfx);
private:
	Vei2 Invalid = { 12345678, 12345678 };
	Vec2 pos;
	static constexpr int radius = 9;
	Color c = Colors::White;
};