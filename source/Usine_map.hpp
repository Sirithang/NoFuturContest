#ifndef _USINEMAP_H
#define _USINEMAP_H

#include <nds.h>
#include "Vector_math.inl"

struct UsineMap
{
	int bg;
	u16 map[12*16];
};

namespace usine
{
	void init(UsineMap& obj);
	Vec2i getCase(UsineMap& obj, touchPosition& position);
	
}

#endif