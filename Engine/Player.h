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
	Vec2 GetPos() const { return pos; }
	void Draw(Graphics& gfx);
private:
	Vec2 pos;
	static constexpr int radius = 9;
	Color c = Colors::White;
};