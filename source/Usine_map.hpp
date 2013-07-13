#ifndef _USINEMAP_H
#define _USINEMAP_H

#include <nds.h>
#include "Vector_math.inl"

//size of a case of the usine map
#define USINE_CASE_T 64
#define NUMBER_MACHINES 12

struct Machine
{
	int x;
	int y;
	int type;

	int workerSprite;
	int workerFrameCounter;
	int workerFrame;
};

struct UsineMap
{
	int bg;
	int w,h;
	u16 map[(SCREEN_WIDTH/USINE_CASE_T) * (SCREEN_HEIGHT/USINE_CASE_T)];

	Machine _machines[NUMBER_MACHINES];
};

namespace usine
{
	void init(UsineMap& obj);
	void update(UsineMap& obj);

	void restoreGraphics(UsineMap& obj);
	void restoreSprite(UsineMap& obj);

	Vec2i getCase(UsineMap& obj, touchPosition& position);
	
}

#endif