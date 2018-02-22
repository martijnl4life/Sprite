#pragma once
#include <vector>
#include "Rect.h"
#include "Vec2.h"
#include "LineSegment.h"
#include "Graphics.h" 


class Layer
{
public:
	Layer() = default;
	void AddRect(const RectI rec)
	{
		rectsInLayer.emplace_back(rec);
	}
	void CalculateWalls()
	{
		if (!rectsInLayer.empty())
		{
			for (auto& r : rectsInLayer)
			{
				walls.emplace_back(Vei2(r.left, r.top), Vei2(r.right, r.top));							// *-->*
				walls.emplace_back(Vei2(r.right, r.top), Vei2(r.right, r.bottom));						// ^   |
				walls.emplace_back(Vei2(r.left, r.bottom), Vei2(r.right, r.bottom));					// *<--* 
				walls.emplace_back(Vei2(r.left, r.top), Vei2(r.left, r.bottom));
			}
			auto temp = walls;
			int count = 0;
			for (auto i = walls.begin(); i < walls.end();)
			{
				count = 0;
				for (auto t = temp.begin(); t < temp.end(); ++t)
				{
					if ((i->p0 == t->p0 && i->p1 == t->p1) || (i->p0 == t->p1 && i->p1 == t->p0))
					{
						count++;
					}
				}
				if (count > 1)
				{
					i = walls.erase(i);
				}
				else
				{
					++i;
				}
			}
			temp = walls;
			std::vector<LineSegment> del;
			bool deleted = false;
			for (auto& i = walls.begin(); i < walls.end();)
			{
				for (auto& t = temp.begin(); t < temp.end();)
				{
					if ((i->p1 == t->p0) && (((i->p0.x == i->p1.x) == (t->p0.x == t->p1.x)) || ((i->p0.y == i->p1.y) == (t->p0.y == t->p1.y))))
					{
						deleted = true;
						i->InitNewLineSegment(i->p0, t->p1);
						del.emplace_back(LineSegment( t->p0, t->p1 ));
						t = temp.erase(t);
						break;
					}
					else
					{
						++t;
					}
				}
				if (!deleted)
				{
					++i;
				}
				else
				{
					deleted = false;
				}
			}
			deleted = false;
			for (auto i = walls.begin(); i < walls.end();)
			{
				for (auto t = del.begin(); t < del.end();)
				{
					if ((i->p0 == t->p0 && i->p1 == t->p1) || (i->p0 == t->p1 && i->p1 == t->p0))
					{
						deleted = true;
						i = walls.erase(i);
						t = del.erase(t);
						break;
					}
					else
					{
						++t;
					}
				}
				if (!deleted)
				{
					++i;
				}
				else
				{
					deleted = false;
				}
			}
		}
	}
	void ClearRects()
	{
		rectsInLayer.clear();
	}
	void ClearWalls()
	{
		walls.clear();
	}
	void Draw(Graphics& gfx)
	{
		if (!rectsInLayer.empty())
		{
			for (auto r : rectsInLayer)
			{
				gfx.DrawRectDim(r.left, r.top, r.GetWidth(), r.GetHeight(), Colors::Green);
			}
		}
	}
	void DrawWalls(Graphics& gfx, const RectI& rec)
	{
		if (!rectsInLayer.empty())
		{
			for (auto r : rectsInLayer)
			{
				if (r.left < rec.left || r.top < rec.top || r.right > rec.right || r.bottom > rec.bottom)
				{
					gfx.DrawRectDim(r.left, r.top, r.GetWidth(), r.GetHeight(), Color{ 15,16,17 });
				}
			}
			for (auto w : walls)
			{
				gfx.DrawLine(w.p0, w.p1, Colors::White);
			}
		}
		
	}
	std::vector<RectI> GetRectVector() 
	{
		return rectsInLayer;
	}
	const std::vector<LineSegment> GetWalls() const
	{
		return walls;
	}
private:
	std::vector<RectI> rectsInLayer;
	std::vector<LineSegment> walls;
};