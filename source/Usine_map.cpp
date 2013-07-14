#include "Usine_map.hpp"
#include "../assets/test_usine_tilesets.h"

#include "../assets/symbole.h"
#include "../assets/work.h"

#include <stdlib.h>
#include <time.h>

#define GROUND_CASE 1
#define MACHINE_1 17

#define WORKER_TILE_SIZE (4*4*32)

#define ANIM_SLEEP (SPRITESHEET_WORK+16*12)


u16 upperSprite = 0;

using namespace usine;

//---------------------------------------------------------------

void workCallback()
{

}

void machine::startWork(Machine& mach)
{

}


//---------------------------------------------------------------

void usine::init(UsineMap& obj)
{
	obj.w = SCREEN_WIDTH/USINE_CASE_T;
	obj.h = SCREEN_HEIGHT/USINE_CASE_T;

	for(int i = 0; i < obj.w*obj.h; ++i)
		obj.map[i] = 0;

	int obstacleSpawned[MAX_OBSTACLE_TYPE];
	for(int i = 0; i < MAX_OBSTACLE_TYPE; ++i)
		obstacleSpawned[i] = 0;

	for(int i = 0; i < obj.w; ++i)
	{
		for(int j = 0; j < obj.h; ++j)
		{
			u8 y = j;
			u8 x = i;

			int type = (rand()%128) / 32;
			int idx = x + y*obj.w;
			obj.map[idx] = type;

			obj._machines[idx].x = x * USINE_CASE_T;
			obj._machines[idx].y = y * USINE_CASE_T;
			obj._machines[idx].type = type;

			int obstRand = rand()%MAX_OBSTACLE_TYPE;
			while(obstacleSpawned[obstRand] > ((idx < 5)?0:2))
			{
				obstRand = rand()%MAX_OBSTACLE_TYPE;
			}

			obstacleSpawned[obstRand] += 1;

			obj._machines[idx].obstacle = (ObstacleType)obstRand;
			obj._machines[idx].iconeSprite = upperSprite++;

			obj._machines[idx].workerSpritesheetOffset = ANIM_SLEEP;
			obj._machines[idx].workerSprite = upperSprite++;
			obj._machines[idx].workerFrame = rand()%6;
			obj._machines[idx].workerFrameCounter = 0;
		}
	}
}

//===================================

Vec2i usine::getCase(UsineMap& obj, touchPosition& position)
{
	Vec2i result;

	result.x = position.px / USINE_CASE_T;
	result.y = position.py / USINE_CASE_T;

	return result;
}

//===================================

void usine::restoreGraphics(UsineMap& obj)
{
	obj.bg = bgInit(0, BgType_Text8bpp, BgSize_T_256x256, 0,1);
	dmaCopy(test_usine_tilesetsTiles,bgGetGfxPtr(obj.bg), test_usine_tilesetsTilesLen);
	dmaCopy(test_usine_tilesetsPal, BG_PALETTE, sizeof(test_usine_tilesetsPal));

	u16* mapPtr = bgGetMapPtr(obj.bg);

	for(int i = 0; i < obj.w * (USINE_CASE_T/8); ++i)
	{
		for(int j = 0; j < obj.h * (USINE_CASE_T/8); ++j)
		{
			u32 idx = i + j * (256/8);

			u8 x = i / (USINE_CASE_T/8);
			u8 y = j / (USINE_CASE_T/8);

			mapPtr[idx] = bgGetMapBase(obj.bg)+ ((obj.map[x + y*obj.w]) * (USINE_CASE_T/8*8)) + ((i%(USINE_CASE_T/8)) + (j%(USINE_CASE_T/8))*(USINE_CASE_T/8));
		}
	}
}

//===================================

void usine::restoreSprite(UsineMap& obj)
{
	dmaCopy(symboleTiles, oamGetGfxPtr(&oamMain, ICONE_SPRITESHEET), sizeof(symboleTiles));
	dmaCopy(workTiles, oamGetGfxPtr(&oamMain, SPRITESHEET_WORK), sizeof(workTiles));

	//dmaCopy(symbolePal, SPRITE_PALETTE, sizeof(symbolePal));
	dmaCopy(workPal, SPRITE_PALETTE, sizeof(workPal));
	dmaCopy(symbolePal, SPRITE_PALETTE + 16, sizeof(symbolePal));
}

void usine::update(UsineMap& obj)
{
	for(int i = 0; i < NUMBER_MACHINES; ++i)
	{
		Machine mach = obj._machines[i];

		mach.workerFrameCounter+=1;
		if ((mach.workerFrameCounter & 0x7) == 0)
		{
			mach.workerFrame = mach.workerFrame < 4 ? mach.workerFrame + 1 : 0;
		}

		oamSet(	&oamMain, mach.workerSprite, /*x*/mach.x+16, /*y*/mach.y+32, /*priority*/0, /*palette*/0,
			SpriteSize_32x32, SpriteColorFormat_16Color, oamGetGfxPtr(&oamMain, mach.workerSpritesheetOffset+ 16*mach.workerFrame),
		0, false, false, false, false, false);

		oamSet( &oamMain, mach.iconeSprite, mach.x, mach.y, 0, 1, SpriteSize_32x16, SpriteColorFormat_16Color, 
			oamGetGfxPtr(&oamMain, (int)mach.obstacle * 8), 0, false, false, false, false, false);

		obj._machines[i] = mach;
	}
}