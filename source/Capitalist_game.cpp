#include <nds.h>
#include "Capitalist_game.hpp"
#include "../assets/capitalist.h"
#include "../assets/bar.h"

#define CAPITALIST_SPRITE (BAR_SPRITE + BAR_SPRITE_COUNT)
#define CAPITALIST_TILE (BAR_TILE + BAR_TILE_COUNT*5)
#define CAPITALIST_TILE_COUNT (8*8)
#define CAPITALIST_TILE_SIZE (CAPITALIST_TILE_COUNT*32)

Capitalist Capitalist::capitalist;

void Capitalist::init()
{
	init_timer(3, 0);
	counter = 0;

	oamClear(&oamMain, 0, 0);
	oamEnable(&oamMain);
	oamInit(&oamMain, SpriteMapping_1D_32, false);

	dmaCopy(capitalistTiles, oamGetGfxPtr(&oamMain, CAPITALIST_TILE), CAPITALIST_TILE_SIZE);
	dmaCopy(capitalistPal, SPRITE_PALETTE, sizeof(capitalistPal));
	
	oamSet(	&oamMain, CAPITALIST_SPRITE, 50, 50, /*priority*/1, /*palette*/0,
		SpriteSize_64x64, SpriteColorFormat_16Color, oamGetGfxPtr(&oamMain, CAPITALIST_TILE),
		-1, false, false, false, false, false);

	Game::init();

	writeTimed("\x1b[10;10HFrappe!", 1000);
}

void Capitalist::update()
{
	// timeout
	Game::update();

	if (keysDown() & KEY_TOUCH)
	{
		touchPosition touch;
		touchRead(&touch);

		SpriteEntry* capitalist = oamMain.oamMemory + CAPITALIST_SPRITE;

		if (	touch.px > capitalist->x && touch.px < capitalist->x + 64 &&
			touch.py > capitalist->y && touch.py < capitalist->y + 64)
		{
			++counter;

			if (counter >= 3)
			{
				game_end(true);
			}
		}
	}
}

void Capitalist::draw()
{
	oamUpdate(&oamMain);
}
