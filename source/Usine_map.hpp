#ifndef _USINEMAP_H
#define _USINEMAP_H

#include <nds.h>
#include "Vector_math.inl"

//size of a case of the usine map
#define USINE_CASE_T 64
#define NUMBER_MACHINES 12

#define ICONE_SPRITESHEET 0
#define SPRITESHEET_WORK ((32*8)) //32 x nbFrame of SLeep

#define ICONE_PALETTE 0
#define WORKER_PALETTE 1


enum ObstacleType { SQUARE, LOSANGE, TRIANGLE, ARROW, DASH, M, CIRCLE, HEART, MAX_OBSTACLE_TYPE };

struct Machine
{
	u8	usable;
	u16 frameUntilUsable;

	int x;
	int y;
	int type;
	ObstacleType obstacle;
	int iconeSprite;

	int workerSprite;
	int workerSpritesheetOffset;
	int workerFramesCount;
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

namespace machine
{
	void startWork(Machine& mach);
}

#endif