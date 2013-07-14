#include "Usine_map.hpp"
#include "../assets/test_usine_tilesets.h"
#include "../assets/background_usine.h"
#include "../assets/symbole.h"
#include "../assets/work.h"

#include <stdlib.h>
#include <time.h>

#define GROUND_CASE 1
#define MACHINE_1 17

#define WORKER_TILE_SIZE (4*4*16)

#define ANIM_PUSH SPRITESHEET_WORK
#define ANIM_PUSH_FRAMES 11

#define ANIM_SLEEP (ANIM_PUSH + 16*12)
#define ANIM_SLEEP_FRAMES 5

#define ANIM_JUMP (ANIM_SLEEP + 16*12)
#define ANIM_JUMP_FRAMES 5

#define ANIM_WORK (ANIM_JUMP + 16*12)
#define ANIM_WORK_FRAMES 5

#define ANIM_PANICK (ANIM_WORK + 16*12)
#define ANIM_PANICK_FRAMES 3


u16 upperSprite = 0;

using namespace usine;

//---------------------------------------------------------------

void machine::startWork(Machine& mach)
{
	const int offset[3] = {ANIM_SLEEP, ANIM_PANICK, ANIM_JUMP};
	const int framesNb[3] = {ANIM_SLEEP_FRAMES, ANIM_PANICK_FRAMES, ANIM_JUMP_FRAMES };

	int idx = rand()%3;

	mach.workerFrame = 0;
	mach.workerSpritesheetOffset = offset[idx];
	mach.workerFramesCount = framesNb[idx];
	mach.workerFrameCounter = 0;

	mach.usable = 0;
	mach.frameUntilUsable = 3*60;
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

			obj._machines[idx].usable = 1;
			obj._machines[idx].frameUntilUsable = 0;

			obj._machines[idx].obstacle = (ObstacleType)obstRand;
			obj._machines[idx].iconeSprite = upperSprite++;

			const int offset[2] = {ANIM_WORK, ANIM_PUSH};
			const int framesNb[2] = {ANIM_WORK_FRAMES, ANIM_PUSH_FRAMES };

			int anim = rand()%2;

			obj._machines[idx].workerSprite = upperSprite++;
			obj._machines[idx].workerSpritesheetOffset = offset[anim];
			obj._machines[idx].workerFramesCount = framesNb[anim];
			obj._machines[idx].workerFrameCounter = 0;
			obj._machines[idx].workerFrame = rand()%framesNb[anim];
			obj._machines[idx].workerPal = rand()%4;

			obj._machines[idx].machinePal = rand()%3;
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
	obj.bg = bgInit(0, BgType_Text4bpp, BgSize_T_256x256, 0,1);
	dmaCopy(test_usine_tilesetsTiles,bgGetGfxPtr(obj.bg), test_usine_tilesetsTilesLen);

	dmaCopy(test_usine_tilesetsPal, BG_PALETTE, 32);
	dmaCopy(test_usine_tilesetsPal+32, BG_PALETTE+16, 32);
	dmaCopy(test_usine_tilesetsPal+64, BG_PALETTE+32, 32);

	int bg2 = bgInit(1, BgType_Text4bpp, BgSize_T_256x256, 1, 2);
	dmaCopy(background_usineTiles, bgGetGfxPtr(bg2), background_usineTilesLen);
	dmaCopy(background_usinePal+96, BG_PALETTE+48, 32);

	u16* mapPtr = bgGetMapPtr(obj.bg);
	u16* map2Ptr = bgGetMapPtr(bg2);

	int lastX = 0;
	int lastY = 0;

	for(int i = 0; i < obj.w * (USINE_CASE_T/8); ++i)
	{
		for(int j = 0; j < obj.h * (USINE_CASE_T/8); ++j)
		{
			u32 idx = i + j * (256/8);

			u8 x = i / (USINE_CASE_T/8);
			u8 y = j / (USINE_CASE_T/8);

			mapPtr[idx] = ((obj.map[x + y*obj.w]) * (USINE_CASE_T/8*8)) + ((i%(USINE_CASE_T/8)) + (j%(USINE_CASE_T/8))*(USINE_CASE_T/8)) | TILE_PALETTE(obj._machines[x + y*obj.w].machinePal);

			map2Ptr[idx] = idx | TILE_PALETTE(2);
		}
	}
}

//===================================

void usine::restoreSprite(UsineMap& obj)
{
	dmaCopy(symboleTiles, oamGetGfxPtr(&oamMain, ICONE_SPRITESHEET), sizeof(symboleTiles));
	dmaCopy(workTiles, oamGetGfxPtr(&oamMain, SPRITESHEET_WORK), sizeof(workTiles));

	//dmaCopy(symbolePal, SPRITE_PALETTE, sizeof(symbolePal));
	dmaCopy(workPal, SPRITE_PALETTE, 32);
	dmaCopy(workPal + 32, SPRITE_PALETTE + 16, 32);
	dmaCopy(workPal + 64, SPRITE_PALETTE + 32, 32);
	dmaCopy(workPal + 96, SPRITE_PALETTE + 48, 32);

	dmaCopy(symbolePal, SPRITE_PALETTE + 64, sizeof(symbolePal));
	
}

void usine::update(UsineMap& obj)
{
	for(int i = 0; i < NUMBER_MACHINES; ++i)
	{
		Machine mach = obj._machines[i];

		if(mach.usable == 0)
		{
			mach.frameUntilUsable--;
			if(mach.frameUntilUsable == 0)
			{
				mach.usable = 1;

				const int offset[2] = {ANIM_WORK, ANIM_PUSH};
				const int framesNb[2] = {ANIM_WORK_FRAMES, ANIM_PUSH_FRAMES };

				int idx = rand()%2;

				mach.workerFrame = 0;
				mach.workerSpritesheetOffset = offset[idx];
				mach.workerFramesCount = framesNb[idx];
				mach.workerFrameCounter = 0;
			}
		}


		mach.workerFrameCounter+=1;
		if ((mach.workerFrameCounter & 0x7) == 0)
		{
			mach.workerFrame = mach.workerFrame < mach.workerFramesCount ? mach.workerFrame + 1 : 0;
		}

		oamSet(	&oamMain, mach.workerSprite, /*x*/mach.x+16, /*y*/mach.y+32, /*priority*/0, /*palette*/mach.workerPal,
			SpriteSize_32x32, SpriteColorFormat_16Color, oamGetGfxPtr(&oamMain, mach.workerSpritesheetOffset+ 16*mach.workerFrame),
		0, false, false, false, false, false);

		oamSet( &oamMain, mach.iconeSprite, mach.x, mach.y, 0, 4, SpriteSize_32x16, SpriteColorFormat_16Color, 
			oamGetGfxPtr(&oamMain, (int)mach.obstacle * 8), 0, false, !mach.usable, false, false, false);

		obj._machines[i] = mach;
	}
}