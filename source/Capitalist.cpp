#include <nds.h>
#include "Capitalist.hpp"
#include "../assets/capitalist.h"

#define CAPITALIST_SPRITE 0
#define CAPITALIST_TILE 0
#define CAPITALIST_TILE_COUNT (8*8)
#define CAPITALIST_TILE_SIZE (CAPITALIST_TILE_COUNT*32)

Capitalist Capitalist::capitalist;

void Capitalist::init()
{
	init_timer(3, 0);
	counter = 0;

	capitalist_x = 50;
	capitalist_y = 50;

	oamClear(&oamMain, 0, 0);
	oamEnable(&oamMain);
	oamInit(&oamMain, SpriteMapping_1D_32, false);

	dmaCopy(capitalistTiles, oamGetGfxPtr(&oamMain, CAPITALIST_SPRITE), CAPITALIST_TILE_SIZE);
	dmaCopy(capitalistPal, SPRITE_PALETTE, sizeof(capitalistPal));
	
	oamSet(	&oamMain, CAPITALIST_SPRITE, capitalist_x, capitalist_y, /*priority*/1, /*palette*/0,
		SpriteSize_64x64, SpriteColorFormat_16Color, oamGetGfxPtr(&oamMain, CAPITALIST_TILE),
		0, false, false, false, false, false);

	bgHide(0);
	bgHide(1);
	bgHide(2);
	bgHide(3);
}

void Capitalist::update()
{
	// timeout
	Game::update();

	if (keysDown() & KEY_TOUCH)
	{
		touchPosition touch;
		touchRead(&touch);

		if (	touch.px > capitalist_x && touch.px < capitalist_x + 64 &&
			touch.py > capitalist_y && touch.py < capitalist_y + 64)
		{
			++counter;

			if (counter >= 3)
			{
				game_end(true);
			}
		}
	}
	
	oamSet(	&oamMain, CAPITALIST_SPRITE, capitalist_x, capitalist_y, /*priority*/1, /*palette*/0,
		SpriteSize_64x64, SpriteColorFormat_16Color, oamGetGfxPtr(&oamMain, CAPITALIST_TILE),
		0, false, false, false, false, false);
}

void Capitalist::draw()
{
	oamUpdate(&oamMain);
}
