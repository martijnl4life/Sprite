#include "Light.h"

void Light::AddLightsource(const Vei2 & lightsource)
{
	lightSources.emplace_back(lightsource);
}

Vei2 Light::RayCast(std::vector<LineSegment> wallVertices, Vec2 direction, Vei2 toPoint, const Vei2& lightSource)
{
	Vei2 closestPoint = toPoint;
	for (int i = 0; i < wallVertices.size(); i++)
	{
		geo::IntersectionObject obj = geo::RayToLineSegmentIntersection(Vec2(lightSource), Vec2(lightSource) + direction, Vec2{ wallVertices[i].p0 }, Vec2{ wallVertices[i].p1 });
		if (!obj.points.empty())
		{
			for (auto v : obj.points)
			{
				const float l0 = (v - Vec2(lightSource)).GetLengthSq();
				const float l1 = (Vec2(closestPoint) - Vec2(lightSource)).GetLengthSq();
				if (l0 < l1)
				{
					closestPoint = Vei2(v);
				}
			}
		}
	}

	return closestPoint;
}

std::vector<LightPoint> Light::LineOfSight(std::vector<LineSegment> wallVertices, const Vei2 & lightSource)
{
	std::vector<LightPoint> closestPoints;
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
		const Vei2 Checked = checked[i];
		Vei2 point = RayCast(wallVertices, (Vec2(Checked) - Vec2(lightSource)).GetNormalized(), Checked, lightSource);
		Vei2 point2 = RayCast(wallVertices, (Vec2(Checked) - Vec2(lightSource)).GetNormalized().Rotate(0.01f), Invalid, lightSource);
		Vei2 point3 = RayCast(wallVertices, (Vec2(Checked) - Vec2(lightSource)).GetNormalized().Rotate(-0.01f), Invalid, lightSource);

		const float angle = std::atan2((float)Checked.y - lightSource.y, (float)Checked.x - lightSource.x);
		closestPoints.emplace_back(point,angle);
		if (!(point2 == point))
		closestPoints.emplace_back(point2,angle+0.0001f);
		if (!(point3 == point))
		closestPoints.emplace_back(point3,angle-0.0001f);
	}
	return closestPoints;
}

void Light::Draw(std::vector<LineSegment> wallVertices, Graphics & gfx)
{
	for (int i = 0; i < lightSources.size(); i++)
	{
		const auto ls = lightSources[i];
		std::vector<LightPoint> lightpoints = LineOfSight(wallVertices, ls);
		std::sort(lightpoints.begin(), lightpoints.end(), [](LightPoint a, LightPoint b) {
			return a.angle < b.angle;
		});
		for (int j = 0; j < lightpoints.size(); j++)
		{
			if (j == lightpoints.size() - 1)
			{
				gfx.DrawTriangle(ls, lightpoints.front().loc, lightpoints.back().loc, Colors::Yellow);
			}
			else 
			{
				gfx.DrawTriangle(ls, lightpoints[j].loc, lightpoints[j + 1].loc, Colors::Yellow);
			}
		}
	}
}
