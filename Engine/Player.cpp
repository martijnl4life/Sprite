#include "Player.h"

Player::Player(Vec2 & pos, Vec2 & dir)
	:
	pos(pos)
{
}

void Player::Update(float dt, const Keyboard& kbd)
{
	if (kbd.KeyIsPressed('A'))
	{
		pos += Vec2(-1, 0) * 5.0f;
	}
	if (kbd.KeyIsPressed('S'))
	{
		pos += Vec2(0, 1) * 5.0f;
	}
	if (kbd.KeyIsPressed('D'))
	{
		pos += Vec2(1, 0) * 5.0f;
	}
	if (kbd.KeyIsPressed('W'))
	{
		pos += Vec2(0, -1) * 5.0f;
	}
}

Vei2 Player::raycasted(std::vector<LineSegment> wallVertices, Vec2 direction, Vei2 toPoint)
{
	Vei2 closestPoint = toPoint;
	Vei2 temp = Invalid;
	std::vector<Vec2> possible;
	for (int i = 0; i < wallVertices.size(); i++)
	{
		geo::IntersectionObject obj = geo::RayToLineSegmentIntersection(pos, pos + direction, Vec2{ wallVertices[i].p0 }, Vec2{ wallVertices[i].p1 });
		if (!obj.points.empty())
		{
			for (auto v : obj.points)
			{
				const float l0 = (v - pos).GetLengthSq();
				const float l1 = (Vec2(closestPoint) - pos).GetLengthSq();
				if (l0 < l1)
				{
					closestPoint = Vei2(v);
				}
			}
		}
	}

	return closestPoint;
}

std::vector<Vei2> Player::LineOfSight(std::vector<LineSegment> wallVertices)
{
	std::vector<Vei2> closestPoints;
	std::vector<Vei2> checked;
	bool checked_p0 = false;
	bool checked_p1 = false;

	for (int i = 0; i < wallVertices.size(); i++)
	{
		for (auto& c : checked)
		{
			if (c == wallVertices[i].p0)
			{
				checked_p0 = true;
			}
			else if (c == wallVertices[i].p1)
			{
				checked_p1 = true;
			}
		}
		if (!checked_p0)
		{
			checked.emplace_back(wallVertices[i].p0);
		}
		if (!checked_p1)
		{
			checked.emplace_back(wallVertices[i].p1);
		}
		checked_p0 = false;
		checked_p1 = false;
	}


	for (int i = 0; i < checked.size(); i++)
	{
		Vei2 point = raycasted(wallVertices, (Vec2(checked[i]) - pos).GetNormalized(), checked[i]);
		closestPoints.emplace_back(point);
	}
	return closestPoints;
}

void Player::Draw(Graphics & gfx)
{
	gfx.DrawCircle((int)pos.x, (int)pos.y, radius, c);
}
