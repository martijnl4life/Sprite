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

Vei2 Player::raycasted(std::vector<LineSegment> wallVertices, Vec2 direction)
{
	Vei2 closestPoint = Invalid;
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
		Vei2 point2 = raycasted(wallVertices, (Vec2(checked[i]) - pos).GetNormalized().Rotate(-0.01f));
		Vei2 point3 = raycasted(wallVertices, (Vec2(checked[i]) - pos).GetNormalized().Rotate(0.01f));
		closestPoints.emplace_back(point);
		closestPoints.emplace_back(point2);
		closestPoints.emplace_back(point3);
	}
	return closestPoints;
}

std::vector<Vei2> Player::LineOfSight2(std::vector<LineSegment> wallVertices)
{
	std::vector<Vei2> closestPoints;
	std::vector<Vei2> uniquePoints;

	bool checked_p0 = false;
	bool checked_p1 = false;

	for (int i = 0; i < wallVertices.size(); i++)
	{
		for (auto& p : uniquePoints)
		{
			if (p == wallVertices[i].p0)
			{
				checked_p0 = true;
			}
			else if (p == wallVertices[i].p1)
			{
				checked_p1 = true;
			}
		}
		if (!checked_p0)
		{
			uniquePoints.emplace_back(wallVertices[i].p0);
		}
		if (!checked_p1)
		{
			uniquePoints.emplace_back(wallVertices[i].p1);
		}
		checked_p0 = false;
		checked_p1 = false;
	}


	std::vector<float> uniqueAngles;
	for (int i = 0; i < uniquePoints.size(); i++)
	{
		Vei2 uniquepoint = uniquePoints[i];
		float angle = std::atan2((float)uniquepoint.y - pos.y, (float)uniquepoint.x - pos.x);
		uniqueAngles.emplace_back(angle - 0.00001f);
		uniqueAngles.emplace_back(angle);
		uniqueAngles.emplace_back(angle + 0.00001f);
	}


	std::vector<geo::Intersect> intersects;
	for (int i = 0; i < uniqueAngles.size(); i++)
	{
		const float angle = uniqueAngles[i];

		const float dx = std::cos(angle);
		const float dy = std::sin(angle);

		const Ray ray = { pos, Vec2(pos.x + dx, pos.y + dy) };

		bool doneOnce = false;
		geo::Intersect closestIntersect = geo::Intersect();
		for (int j = 0; j < wallVertices.size(); j++)
		{
			geo::Intersect intersect = geo::getIntersect(ray, wallVertices[j]);
			if (!intersect.DoesIntersect) continue;
			if (!doneOnce)
			{
				closestIntersect = intersect;
				doneOnce = true;
			}
			if (!closestIntersect.DoesIntersect || intersect.param < closestIntersect.param)
			{
				closestIntersect = intersect;
			}
		}

		if (!closestIntersect.DoesIntersect) continue;

		closestIntersect.angle = angle;

		intersects.emplace_back(closestIntersect);

	}

	std::sort(intersects.begin(), intersects.end(), geo::sortAnglevalue);

	for (int i = 0; i < intersects.size(); i++)
	{
		closestPoints.emplace_back((int)intersects[i].x, (int)intersects[i].y);
	}
	return closestPoints;
}


void Player::Draw(Graphics & gfx)
{
	gfx.DrawCircle((int)pos.x, (int)pos.y, radius, c);
}
