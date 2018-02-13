#include <vector>
#include <algorithm>
#include <cmath>

#include "Vec2.h"
#include "Ray.h"
#include "LineSegment.h"

namespace geo
{
	struct Intersect
	{
		Intersect() = default;
		Intersect(float x, float y, float param, bool intersect)
			:
			x(x),
			y(y),
			param(param),
			DoesIntersect(intersect)
		{}
		Intersect(float angle)
			:
			angle(angle)
		{}
		Intersect(bool intersect)
			:
			DoesIntersect(intersect)
		{}
		float x;
		float y;
		float param;
		float angle;
		bool DoesIntersect;
	};

	static Intersect getIntersect(Ray ray, LineSegment segment)
	{
		const float r_px = (float)ray.p0.x;
		const float r_py = (float)ray.p0.y;
		const float r_dx = (float)ray.p1.x - r_px;
		const float r_dy = (float)ray.p1.x - r_py;

		const float s_px = (float)segment.p0.x;
		const float s_py = (float)segment.p0.y;
		const float s_dx = (float)segment.p1.x - s_px;
		const float s_dy = (float)segment.p1.y - s_py;

		const float r_mag = std::sqrtf(r_dx * r_dx + r_dy * r_dy);
		const float s_mag = std::sqrtf(s_dx * s_dx + s_dy * s_dy);
		if (r_dx / r_mag == s_dx / s_mag && r_dy / r_mag == s_dy / s_mag) return { Intersect(false) };

		const float T2 = (r_dx*(s_py - r_py) + r_dy * (r_px - s_px)) / (s_dx*r_dy - s_dy * r_dx);
		const float T1 = (s_px + s_dx * T2 - r_px) / r_dx;

		if (T1 < 0)
		{
			return Intersect(false);
		}
		if (T2 < 0 || T2 > 1)
		{
			return Intersect(false);
		}
		return Intersect(r_px + r_dx * T1, r_px + r_dx * T1, T1, true);
	}

	static struct sortAnglevalue {
		bool operator() (Intersect i1, Intersect i2) { return (i1.angle < i2.angle); }
	} sortAnglevalue;

	struct IntersectionObject
	{
		std::vector<Vec2> points;

		void InsertSolution(float x, float y)
		{
			points.push_back(Vec2(x, y));
		}

		void InsertSolution(Vec2 v)
		{
			points.push_back(v);
		}

		int NumberOfSolutions()
		{
			return (int)points.size();
		}
	};
	// angle between two Vectors
	static float AngleTwoVectors(Vec2 vertex0, Vec2 vertex1)
	{
		return std::acos(vertex0.Dot(vertex1) / vertex0.GetLength() * vertex1.GetLength());
	}
	static float minimum_distance(Vec2 v, Vec2 w, Vec2 p) {
		// Return minimum distance between line segment vw and point p
		const float l2 = (w - v).GetLengthSq();  // i.e. |w-v|^2 -  avoid a sqrt
		if (l2 == 0.0) return (v - p).GetLength();   // v == w case
													 // Consider the line extending the segment, parameterized as v + t (w - v).
													 // We find projection of point p onto the line. 
													 // It falls where t = [(p-v) . (w-v)] / |w-v|^2
													 // We clamp t from [0,1] to handle points outside the segment vw.
		const float t = std::max(0.0f, std::min(1.0f, (p - v).Dot(w - v) / l2));
		const Vec2 projection = v + (w - v) * t;  // Projection falls on the segment
		return (projection - p).GetLength();
	}

	// Circle to Circle
	static IntersectionObject CircleToCircleIntersection(Vec2 circlePosition1, float radius1,
		Vec2 circlePosition2, float radius2)
	{
		IntersectionObject result;
		float a, distSq, dist, h;
		Vec2 d, r, v2;

		// d is the vertical and horizontal distances between the circle centers
		d = circlePosition1 - circlePosition2;

		//distance squared between the circles
		distSq = d.GetLengthSq();

		// Check for equality and infinite intersections exist
		if (distSq == 0 && radius1 == radius2)
		{
			return result;
		}

		//Check for solvability
		if (distSq > (radius1 + radius2) * (radius1 + radius2))
		{
			// no solution. circles do not intersect
			return result;
		}

		if (distSq < abs(radius1 - radius2) * abs(radius1 - radius2))
		{
			// no solution. one circle is contained in the other
			return result;
		}

		if (distSq == (radius1 + radius2) * (radius1 + radius2))
		{
			//one solution
			result.InsertSolution((circlePosition1 - circlePosition2) / (radius1 + radius2) * radius1 + circlePosition1);
			return result;
		}

		dist = sqrt(distSq);

		// 'point 2' is the point where the line through the circle
		// intersection points crosses the line between the circle
		// centers.

		// Determine the distance from point 0 to point 2
		a = ((radius1 * radius1) - (radius2 * radius2) + distSq) / (2.0f * dist);

		// Determine the coordinates of point 2
		v2 = circlePosition1 + d * (a / dist);

		// Determine the distance from point 2 to either of the intersection points
		h = sqrt((radius1 * radius1) - (a * a));

		// Now determine the offsets of the intersection points from point 2
		r = d * (h / dist);
		r = r.Normalize();

		// Determine the absolute intersection points
		result.InsertSolution(v2 + r);
		result.InsertSolution(v2 - r);

		return result;
	}

	// Used only within this namespace
	static int PrivateLineToCircleIntersection(Vec2 vertex1, Vec2 vertex2,
		Vec2 circlePosition, float radius,
		Vec2 & solution1, Vec2 & solution2)
	{
		// Vector from point 1 to point 2
		Vec2 vertex1to2 = vertex2 - vertex1;
		// Vector from point 1 to the circle's center
		Vec2 circleToVertex1 = circlePosition - vertex1;

		float dot = vertex1to2.Dot(circleToVertex1);
		Vec2 proj1 = vertex1to2 * (dot / vertex1to2.GetLengthSq());

		Vec2 midpt = vertex1 + proj1;
		Vec2 circleToMidpt = midpt - circlePosition;
		float distSqToCenter = circleToMidpt.GetLengthSq();
		if (distSqToCenter > radius * radius) return 0;
		if (distSqToCenter == radius * radius)
		{
			solution1 = midpt;
			return 1;
		}
		float distToIntersection;
		if (distSqToCenter == 0)
		{
			distToIntersection = radius;
		}
		else
		{
			distToIntersection = sqrt(radius * radius - distSqToCenter);
		}
		vertex1to2 = vertex1to2.Normalize();
		vertex1to2 *= distToIntersection;

		solution1 = midpt + vertex1to2;
		solution2 = midpt - vertex1to2;
		return 2;
	}

	//Line to Circle
	static IntersectionObject LineToCircleIntersection(Vec2 vertex1, Vec2 vertex2,
		Vec2 circlePosition, float radius)
	{
		IntersectionObject result;
		Vec2 solution1, solution2;
		switch (PrivateLineToCircleIntersection(vertex1, vertex2, circlePosition, radius, solution1, solution2))
		{
		case 2:
			result.InsertSolution(solution2);
		case 1:
			result.InsertSolution(solution1);
			break;
		}
		return result;
	}

	// Circle to Line
	static IntersectionObject CircleToLineIntersection(Vec2 circlePosition, float radius,
		Vec2 vertex1, Vec2 vertex2)
	{
		return LineToCircleIntersection(vertex1, vertex2, circlePosition, radius);
	}

	// LineSegment to Circle
	static IntersectionObject LineSegmentToCircleIntersection(Vec2 vertex1, Vec2 vertex2,
		Vec2 circlePosition, float radius)
	{
		IntersectionObject result;
		Vec2 solution1, solution2;
		Vec2 vertex1to2 = vertex2 - vertex1;
		Vec2 vertex1ToSolution1, vertex2ToSolution1, vertex1ToSolution2, vertex2ToSolution2;
		switch (PrivateLineToCircleIntersection(vertex1, vertex2, circlePosition, radius, solution1, solution2))
		{
		case 2:
			vertex1ToSolution2 = solution2 - vertex1;
			vertex2ToSolution2 = solution2 - vertex2;
			if (vertex1ToSolution2.Dot(vertex1to2) > 0 &&
				vertex2ToSolution2.Dot(vertex1to2) < 0)
			{
				result.InsertSolution(solution2);
			}
		case 1:
			vertex1ToSolution1 = solution1 - vertex1;
			vertex2ToSolution1 = solution1 - vertex2;
			if (vertex1ToSolution1.Dot(vertex1to2) > 0 &&
				vertex2ToSolution1.Dot(vertex1to2) < 0)
			{
				result.InsertSolution(solution1);
			}
			break;
		}
		return result;
	}

	// Circle to LineSegment
	static IntersectionObject CircleToLineSegmentIntersection(Vec2 circlePosition, float radius,
		Vec2 vertex1, Vec2 vertex2)
	{
		return LineSegmentToCircleIntersection(vertex1, vertex2, circlePosition, radius);
	}

	// Ray to Circle
	static IntersectionObject RayToCircleIntersection(Vec2 vertex1, Vec2 vertex2,
		Vec2 circlePosition, float radius)
	{
		IntersectionObject result;
		Vec2 solution1, solution2;
		Vec2 vertex1to2 = vertex2 - vertex1;
		Vec2 vertex1ToSolution1, vertex1ToSolution2;
		switch (PrivateLineToCircleIntersection(vertex1, vertex2, circlePosition, radius, solution1, solution2))
		{
		case 2:
			vertex1ToSolution2 = solution2 - vertex1;
			if (vertex1ToSolution2.Dot(vertex1to2) > 0)
			{
				result.InsertSolution(solution2);
			}
		case 1:
			vertex1ToSolution1 = solution1 - vertex1;
			if (vertex1ToSolution1.Dot(vertex1to2) > 0)
			{
				result.InsertSolution(solution1);
			}
			break;
		}
		return result;
	}

	// Circle to Ray
	static IntersectionObject CircleToRayIntersection(Vec2 circlePosition, float radius,
		Vec2 vertex1, Vec2 vertex2)
	{
		return RayToCircleIntersection(vertex1, vertex2, circlePosition, radius);
	}

	// Used only within this namespace
	static bool PrivateLineToLineIntersection(Vec2 vertex1, Vec2 vertex2,
		Vec2 vertex3, Vec2 vertex4, float & r, float & s)
	{
		float d;
		//Make sure the lines aren't parallel
		Vec2 vertex1to2 = vertex2 - vertex1;
		Vec2 vertex3to4 = vertex4 - vertex3;
		//if (vertex1to2.x * -vertex3to4.y + vertex1to2.y * vertex3to4.x != 0)
		//{
		if (vertex1to2.y / vertex1to2.x != vertex3to4.y / vertex3to4.x)
		{
			d = vertex1to2.x * vertex3to4.y - vertex1to2.y * vertex3to4.x;
			if (d != 0)
			{
				Vec2 vertex3to1 = vertex1 - vertex3;
				r = (vertex3to1.y * vertex3to4.x - vertex3to1.x * vertex3to4.y) / d;
				s = (vertex3to1.y * vertex1to2.x - vertex3to1.x * vertex1to2.y) / d;
				return true;
			}
		}
		return false;
	}

	// Line to Line
	static IntersectionObject LineToLineIntersection(Vec2 vertex1, Vec2 vertex2,
		Vec2 vertex3, Vec2 vertex4)
	{
		IntersectionObject result;
		float r, s;
		if (PrivateLineToLineIntersection(vertex1, vertex2, vertex3, vertex4, r, s))
		{
			result.InsertSolution(vertex1 + (vertex2 - vertex1) * r);
		}
		return result;
	}

	// LineSegment to LineSegment
	static IntersectionObject LineSegmentToLineSegmentIntersection(Vec2 vertex1, Vec2 vertex2,
		Vec2 vertex3, Vec2 vertex4)
	{
		IntersectionObject result;
		float r, s;
		if (PrivateLineToLineIntersection(vertex1, vertex2, vertex3, vertex4, r, s))
		{
			if (r >= 0 && r <= 1)
			{
				if (s >= 0 && s <= 1)
				{
					result.InsertSolution(vertex1 + (vertex2 - vertex1) * r);
				}
			}
		}
		return result;
	}

	// LineSegment to Line
	static IntersectionObject LineSegmentToLineIntersection(Vec2 vertex1, Vec2 vertex2,
		Vec2 vertex3, Vec2 vertex4)
	{
		IntersectionObject result;
		float r, s;
		if (PrivateLineToLineIntersection(vertex1, vertex2, vertex3, vertex4, r, s))
		{
			if (r >= 0 && r <= 1)
			{
				result.InsertSolution(vertex1 + (vertex2 - vertex1) * r);
			}
		}
		return result;
	}

	// Line to LineSement
	static IntersectionObject LineToLineSegmentIntersection(Vec2 vertex1, Vec2 vertex2,
		Vec2 vertex3, Vec2 vertex4)
	{
		return LineSegmentToLineIntersection(vertex3, vertex4, vertex1, vertex2);
	}

	// Ray to LineSegment
	static IntersectionObject RayToLineSegmentIntersection(Vec2 vertex1, Vec2 vertex2,
		Vec2 vertex3, Vec2 vertex4)
	{
		IntersectionObject result;
		float r, s;
		if (PrivateLineToLineIntersection(vertex1, vertex2, vertex3, vertex4, r, s))
		{
			if (r >= 0)
			{
				if (s >= 0 && s <= 1)
				{
					result.InsertSolution(vertex1 + (vertex2 - vertex1) * r);
				}
			}
		}
		return result;
	}

	// LineSegment to Ray
	static IntersectionObject LineSegmentToRayIntersection(Vec2 vertex1, Vec2 vertex2,
		Vec2 vertex3, Vec2 vertex4)
	{
		return RayToLineSegmentIntersection(vertex3, vertex4, vertex1, vertex2);
	}

	// Ray to Line
	static IntersectionObject RayToLineIntersection(Vec2 vertex1, Vec2 vertex2,
		Vec2 vertex3, Vec2 vertex4)
	{
		IntersectionObject result;
		float r, s;
		if (PrivateLineToLineIntersection(vertex1, vertex2, vertex3, vertex4, r, s))
		{
			if (r >= 0)
			{
				result.InsertSolution(vertex1 + (vertex2 - vertex1) * r);
			}
		}
		return result;
	}

	// Line to Ray
	static IntersectionObject LineToRayIntersection(Vec2 vertex1, Vec2 vertex2,
		Vec2 vertex3, Vec2 vertex4)
	{
		return RayToLineIntersection(vertex3, vertex4, vertex1, vertex2);
	}

	// Ray to Ray
	static IntersectionObject RayToRayIntersection(Vec2 vertex1, Vec2 vertex2,
		Vec2 vertex3, Vec2 vertex4)
	{
		IntersectionObject result;
		float r, s;
		if (PrivateLineToLineIntersection(vertex1, vertex2, vertex3, vertex4, r, s))
		{
			if (r >= 0)
			{
				if (s >= 0)
				{
					result.InsertSolution(vertex1 + (vertex2 - vertex1) * r);
				}
			}
		}
		return result;
	}
}