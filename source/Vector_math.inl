#ifndef _VECTORMATH_H
#define _VECTORMATH_H

struct Vec2i
{
	int x,y;
};

namespace vec2i
{
	inline Vec2i create(int x, int y)
	{
		Vec2i vec;
		vec.x = x;
		vec.y = y;
		return vec;
	}
}

#endif