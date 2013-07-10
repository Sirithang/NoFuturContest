#ifndef OBJECT_H
#define OBJECT_H

struct Object
{
	int x,y;
	int sprite;
	int tile;
	int palette;
};

namespace object
{
	inline void setPos(Object& obj, int x, int y)
	{
		obj.x = x;
		obj.y = y;
	}
}

#endif