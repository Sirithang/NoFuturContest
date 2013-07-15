#include "Aim_game.hpp"
#include "../assets/capitalist.h"
#include "../assets/soviet.h"
#include "../assets/bar.h"

#include <maxmod9.h>    // Maxmod definitions for ARM9
#include "../assets/soundbank.h"
#include "../assets/soundbank_bin.h"  // Soundbank definitions

#define CAPITALIST_TILE (BAR_TILE + BAR_TILE_COUNT*5)
#define CAPITALIST_TILE_COUNT (8*8)
#define CAPITALIST_TILE_SIZE (CAPITALIST_TILE_COUNT*32)

#define SOVIET_SPRITE (BAR_SPRITE + BAR_SPRITE_COUNT)
#define SOVIET_TILE (CAPITALIST_TILE + 2*CAPITALIST_TILE_COUNT)
#define SOVIET_TILE_COUNT (8*8)
#define SOVIET_TILE_SIZE (SOVIET_TILE_COUNT*32)

Aim Aim::aim;

void Aim::init()
{
	init_timer(3, 0);
	counter = 0;

	oamClear(&oamMain, 0, 0);
	oamEnable(&oamMain);
	oamInit(&oamMain, SpriteMapping_1D_32, false);

	dmaCopy(capitalistTiles, oamGetGfxPtr(&oamMain, CAPITALIST_TILE), sizeof(capitalistTiles));
	dmaCopy(capitalistPal, SPRITE_PALETTE, sizeof(capitalistPal));
	dmaCopy(sovietTiles, oamGetGfxPtr(&oamMain, SOVIET_TILE), sizeof(sovietTiles));
	dmaCopy(sovietPal, SPRITE_PALETTE + 16, sizeof(sovietPal));

	for (int i = 0; i <= NUM_SOVIET; ++i)
	{
		frame[i] = rand() % 8;
		anim[i] = rand() % 2;

		oamSet(	&oamMain, SOVIET_SPRITE + i, (rand() % 128) + 32, (rand() % 64) + 32, /*priority*/1, /*palette*/1,
			SpriteSize_64x64, SpriteColorFormat_16Color, NULL,
			-1, false, false, false, false, false);
	}

	for (int i = 0; i < NUM_SOVIET; ++i)
	{
		SpriteEntry* ii = oamMain.oamMemory + SOVIET_SPRITE + i;

		for (int j = i + 1; j <= NUM_SOVIET; ++j)
		{
			SpriteEntry* jj = oamMain.oamMemory + SOVIET_SPRITE + j;
			
			if (ii->y < jj->y)
			{
				int tmp = jj->y;
				jj->y = ii->y;
				ii->y = tmp;
			} 
		}
	}

	capitalist_sprite = rand() % NUM_SOVIET;
	(oamMain.oamMemory + SOVIET_SPRITE + capitalist_sprite)->palette = 0;

	for (int i = 0; i <= NUM_SOVIET; ++i)
	{
		if (i == capitalist_sprite)
		{
			(oamMain.oamMemory + SOVIET_SPRITE + i)->gfxIndex = CAPITALIST_TILE + anim[i]*CAPITALIST_TILE_COUNT;
		}
		else
		{
			(oamMain.oamMemory + SOVIET_SPRITE + i)->gfxIndex = SOVIET_TILE + anim[i]*SOVIET_TILE_COUNT;
		}
	}

	Game::init();

	writeTimed("\x1b[5;15HCamoufle le capitaliste!", 1000);
}

void Aim::update()
{
	// timeout
	Game::update();

	if (!finished)
	{
		if (keysDown() & KEY_TOUCH)
		{
			touchPosition touch;
			touchRead(&touch);

			SpriteEntry* capitalist = oamMain.oamMemory + SOVIET_SPRITE + capitalist_sprite;

			if (	touch.px > capitalist->x && touch.px < capitalist->x + 64 &&
				touch.py > capitalist->y && touch.py < capitalist->y + 64)
			{
				mmEffect( SFX_YEEHAW );
				game_end(true);
				capitalist->gfxIndex = SOVIET_TILE + anim[capitalist_sprite]*SOVIET_TILE_COUNT;
				capitalist->palette = 1;
				capitalist_sprite = -1;
			}
		}
	}

	for (int i = 0; i <= NUM_SOVIET; ++i)
	{
		++frame[i];
		if (frame[i] == 8)
		{
			frame[i] = 0;
			anim[i] = (anim[i] + 1) % 2;

			if (i == capitalist_sprite)
			{
				(oamMain.oamMemory + SOVIET_SPRITE + i)->gfxIndex = CAPITALIST_TILE + anim[i]*CAPITALIST_TILE_COUNT;
			}
			else
			{
				(oamMain.oamMemory + SOVIET_SPRITE + i)->gfxIndex = SOVIET_TILE + anim[i]*SOVIET_TILE_COUNT;
			}
		}
	}
}

void Aim::draw()
{
	oamUpdate(&oamMain);
}

