#include "Goulag_game.hpp"
#include "../assets/goulag.h"
#include "../assets/capitalist.h"
#include "../assets/bar.h"

#include <maxmod9.h>    // Maxmod definitions for ARM9
#include "../assets/soundbank.h"
#include "../assets/soundbank_bin.h"  // Soundbank definitions

#define CAPITALIST_SPRITE (BAR_SPRITE + BAR_SPRITE_COUNT)
#define CAPITALIST_TILE (BAR_TILE + BAR_TILE_COUNT*5)
#define CAPITALIST_TILE_COUNT (8*8)
#define CAPITALIST_TILE_SIZE (CAPITALIST_TILE_COUNT*32)

Goulag Goulag::goulag;

void Goulag::init()
{
	init_timer(15, 0);
	counter = 0;

	bg = bgInit(3, BgType_Text4bpp, BgSize_T_256x256, 1,2);
	bgSetPriority(bg, 3);
	dmaCopy(goulagTiles, bgGetGfxPtr(bg), sizeof(goulagTiles));
	dmaCopy(goulagMap, bgGetMapPtr(bg), sizeof(goulagMap));
	dmaCopy(goulagPal, BG_PALETTE + 16, sizeof(goulagPal));

	oamClear(&oamMain, 0, 0);
	oamEnable(&oamMain);
	oamInit(&oamMain, SpriteMapping_1D_32, false);

	dmaCopy(capitalistTiles, oamGetGfxPtr(&oamMain, CAPITALIST_TILE), sizeof(capitalistTiles));
	dmaCopy(capitalistPal, SPRITE_PALETTE, sizeof(capitalistPal));

	for (int i = 0; i < NUM_CAPITALIST; ++i)
	{
		frame[i] = rand() % 8;
		anim[i] = rand() % 2;

		oamSet(	&oamMain, CAPITALIST_SPRITE + i, (rand() % 128) + 32, (rand() % 64) + 16, /*priority*/1, /*palette*/0,
			SpriteSize_64x64, SpriteColorFormat_16Color, oamGetGfxPtr(&oamMain, CAPITALIST_TILE + anim[i]*CAPITALIST_TILE_COUNT),
			-1, false, false, false, false, false);
	}

	for (int i = 0; i < NUM_CAPITALIST - 1; ++i)
	{
		SpriteEntry* ii = oamMain.oamMemory + CAPITALIST_SPRITE + i;

		for (int j = i + 1; j < NUM_CAPITALIST; ++j)
		{
			SpriteEntry* jj = oamMain.oamMemory + CAPITALIST_SPRITE + j;
			
			if (ii->y < jj->y)
			{
				int tmp = jj->y;
				jj->y = ii->y;
				ii->y = tmp;
			} 
		}
	}

	selected_capitalist = -1;

	Game::init();

	writeTimed("\x1b[10;10HSors les capitalists du goulag!", 1000);
}

void Goulag::cleanup()
{
	bgHide(bg);
}

void Goulag::update()
{
	// timeout
	Game::update();

	if (!finished)
	{
		if (keysDown() & KEY_TOUCH)
		{
			touchPosition touch;
			touchRead(&touch);

			selected_capitalist = -1;
			for (int i = NUM_CAPITALIST - 1; i >= 0; --i)
			{
				SpriteEntry* capitalist = oamMain.oamMemory + CAPITALIST_SPRITE + i;

				if (	touch.px >= capitalist->x && touch.px <= capitalist->x + 64 &&
					touch.py >= capitalist->y && touch.py <= capitalist->y + 64)
				{
					selected_capitalist = i;
				}
			}

			select_x = touch.px;
			select_y = touch.py;
		}
		else if ((keysHeld() & KEY_TOUCH) && (selected_capitalist >= 0))
		{
			touchPosition touch;
			touchRead(&touch);

			SpriteEntry* capitalist = oamMain.oamMemory + CAPITALIST_SPRITE + selected_capitalist;
			bool inside = checkin(capitalist);

			capitalist->x += touch.px - select_x;
			capitalist->y += touch.py - select_y;

			capitalist->x = capitalist->x > 0 ? capitalist->x < 192 ? capitalist->x : 192 : 0;
			capitalist->y = capitalist->y > 0 ? capitalist->y < 128 ? capitalist->y : 128 : 0;

			bool inside_new = checkin(capitalist);
			
			if (!inside && inside_new)
			{
				--counter;
			}
			else if (inside && !inside_new)
			{
				++counter;
				if (counter == NUM_CAPITALIST)
				{
					game_end(true);
				}
			}

			unsigned short tmp;
			#define swap(a, b)\
				tmp = a;\
				a = b;\
				b = tmp;
			if (select_y < touch.py)
			{
				for (int i = selected_capitalist - 1; i >= 0; --i)
				{
					SpriteEntry* ii = oamMain.oamMemory + CAPITALIST_SPRITE + i;
					
					if (ii->y < capitalist->y)
					{
						swap(ii->y, capitalist->y);
						swap(ii->x, capitalist->x);
						swap(ii->gfxIndex, capitalist->gfxIndex);
						swap(frame[i], frame[selected_capitalist]);
						swap(anim[i], anim[selected_capitalist]);
						selected_capitalist = i;
						capitalist = oamMain.oamMemory + CAPITALIST_SPRITE + selected_capitalist;
					}
				}
			}
			else if (select_y > touch.py)
			{
				for (int i = selected_capitalist + 1; i < NUM_CAPITALIST; ++i)
				{
					SpriteEntry* ii = oamMain.oamMemory + CAPITALIST_SPRITE + i;
					
					if (ii->y > capitalist->y)
					{
						swap(ii->y, capitalist->y);
						swap(ii->x, capitalist->x);
						swap(ii->gfxIndex, capitalist->gfxIndex);
						swap(frame[i], frame[selected_capitalist]);
						swap(anim[i], anim[selected_capitalist]);
						selected_capitalist = i;
						capitalist = oamMain.oamMemory + CAPITALIST_SPRITE + selected_capitalist;
					}
				}
			}

			select_x = touch.px;
			select_y = touch.py;
		}
	}

	for (int i = 0; i < NUM_CAPITALIST; ++i)
	{
		++frame[i];
		if (frame[i] == 8)
		{
			frame[i] = 0;
			anim[i] = (anim[i] + 1) % 2;

			(oamMain.oamMemory + CAPITALIST_SPRITE + i)->gfxIndex = CAPITALIST_TILE + anim[i]*CAPITALIST_TILE_COUNT;
		}
	}
}

void Goulag::draw()
{
	oamUpdate(&oamMain);
}

bool Goulag::checkin(const SpriteEntry* capitalist)
{
	return capitalist->x > 24 && capitalist->x < 168 && capitalist->y < 88;
}

