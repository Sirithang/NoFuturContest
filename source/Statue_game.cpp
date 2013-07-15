#include <nds.h>
#include "Statue_game.hpp"
#include "../assets/statue.h"
#include "../assets/bar.h"

#include <maxmod9.h>    // Maxmod definitions for ARM9
#include "../assets/soundbank.h"
#include "../assets/soundbank_bin.h"  // Soundbank definitions

#define STATUE_SPRITE (BAR_SPRITE + BAR_SPRITE_COUNT)
#define STATUE_TILE (BAR_TILE + BAR_TILE_COUNT*5)
#define STATUE_TILE_COUNT (4*8)
#define STATUE_TILE_SIZE (STATUE_TILE_COUNT*32)

Statue Statue::statue;
mm_sfxhand sfxhandle_statue_hit;

void Statue::init()
{
	init_timer(1, 30);
	counter = 0;

	oamClear(&oamMain, 0, 0);
	oamEnable(&oamMain);
	oamInit(&oamMain, SpriteMapping_1D_32, false);

	dmaCopy(statuePal, SPRITE_PALETTE, sizeof(statuePal));
	
	oamSet(	&oamMain, STATUE_SPRITE, 50, 50, /*priority*/1, /*palette*/0,
		SpriteSize_32x64, SpriteColorFormat_16Color, oamGetGfxPtr(&oamMain, STATUE_TILE),
		-1, false, false, false, false, false);

	Game::init();

	writeTimed("\x1b[10;10HDétruis!", 1000);
}

void Statue::update()
{
	// timeout
	Game::update();

	if (!finished)
	{
		if (keysDown() & KEY_TOUCH)
		{
			touchPosition touch;
			touchRead(&touch);

			SpriteEntry* statue = oamMain.oamMemory + STATUE_SPRITE;

			if (	touch.px > statue->x && touch.px < statue->x + 64 &&
				touch.py > statue->y && touch.py < statue->y + 64)
			{
				++counter;

				// play hit sfx with random pitch
				sfxhandle_statue_hit = mmEffect( SFX_STATUE_HIT );
				mmEffectRate( sfxhandle_statue_hit, 1024 + (rand() % 256 - 128) );

				if (counter >= 3)
				{
					mmEffect( SFX_STATUE_BREAK );
					game_end(true);
				}
			}
		}
	}
}

void Statue::draw()
{
	dmaCopy(statueTiles + counter*STATUE_TILE_SIZE, oamGetGfxPtr(&oamMain, STATUE_TILE), STATUE_TILE_SIZE);

	oamUpdate(&oamMain);
}
