#include <nds.h>
#include "Capitalist_game.hpp"
#include "../assets/soviet.h"
#include "../assets/bar.h"

#define SOVIET_SPRITE (BAR_SPRITE + BAR_SPRITE_COUNT)
#define SOVIET_TILE (BAR_TILE + BAR_TILE_COUNT*5)
#define SOVIET_TILE_COUNT (8*8)
#define SOVIET_TILE_SIZE (SOVIET_TILE_COUNT*32)

Capitalist Capitalist::capitalist;

void Capitalist::init()
{
	init_timer(3, 0);
	counter = 0;

	frame = 0;
	anim = 0;

	oamClear(&oamMain, 0, 0);
	oamEnable(&oamMain);
	oamInit(&oamMain, SpriteMapping_1D_32, false);

	dmaCopy(sovietTiles, oamGetGfxPtr(&oamMain, SOVIET_TILE), SOVIET_TILE_SIZE);
	dmaCopy(sovietPal, SPRITE_PALETTE, sizeof(sovietPal));
	
	oamSet(	&oamMain, SOVIET_SPRITE, 50, 50, /*priority*/1, /*palette*/0,
		SpriteSize_64x64, SpriteColorFormat_16Color, oamGetGfxPtr(&oamMain, SOVIET_TILE),
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

		SpriteEntry* soviet = oamMain.oamMemory + SOVIET_SPRITE;

		if (	touch.px > soviet->x && touch.px < soviet->x + 64 &&
			touch.py > soviet->y && touch.py < soviet->y + 64)
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

	++frame;
	if (frame == 8)
	{
		frame = 0;
		anim = (anim + 1) % 2;
		dmaCopy(sovietTiles + anim*SOVIET_TILE_SIZE, oamGetGfxPtr(&oamMain, SOVIET_TILE), SOVIET_TILE_SIZE);
	}
}
