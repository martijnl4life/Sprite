#pragma once

#include <cmath>

template<typename T>
class Vec2_
{
public:
	Vec2_() = default;
	Vec2_( T x_in,T y_in )
		:
		x( x_in ),
		y( y_in )
	{}
	template<typename S>
	explicit Vec2_( const Vec2_<S>& src )
		:
		x( (T)src.x ),
		y( (T)src.y )
	{}
	bool operator==(const Vec2_& rhs)
	{
		return (x == rhs.x) && (y == rhs.y);
	}
	Vec2_ operator+( const Vec2_& rhs ) const
	{
		return Vec2_( x + rhs.x,y + rhs.y );
	}
	Vec2_& operator+=( const Vec2_& rhs )
	{
		return *this = *this + rhs;
	}
	Vec2_ operator*( T rhs ) const
	{
		return Vec2_( x * rhs,y * rhs );
	}
	Vec2_& operator*=( T rhs )
	{
		return *this = *this * rhs;
	}
	Vec2_ operator-( const Vec2_& rhs ) const
	{
		return Vec2_( x - rhs.x,y - rhs.y );
	}
	Vec2_& operator-=( const Vec2_& rhs )
	{
		return *this = *this - rhs;
	}
	Vec2_ operator/(T rhs) const
	{
		return Vec2_(x / rhs, y / rhs);
	}
	Vec2_& operator/=(T rhs)
	{
		return *this = *this / rhs;
	}
	T GetLength() const
	{
		return (T)std::sqrt( GetLengthSq() );
	}
	T GetLengthSq() const
	{
		return x * x + y * y;
	}
	Vec2_& Normalize()
	{
		return *this = GetNormalized();
	}
	Vec2_ GetNormalized() const
	{
		const T len = GetLength();
		if( len != (T)0 )
		{
			return *this * ((T)1 / len);
		}
		return *this;
	}
	T Dot(const Vec2_& vec2)
	{
		return T((x*vec2.x) + (y * vec2.y));
	}
	Vec2_ Rotated(float angleDegree)
	{
		const double theta = angleDegree * M_PI / 180;
		const T cs = (float)cos(theta);
		const T sn = (float)sin(theta);
		const T px = x * cs - y * sn;
		const T py = x * sn + y * cs;

		return Vec2_<T>(px, py);
	}
	Vec2_& Rotate(float angleDegree)
	{
		return *this = Rotated(angleDegree);
	}
public:
	T x;
	T y;
};

typedef Vec2_<float> Vec2;
typedef Vec2_<int> Vei2;