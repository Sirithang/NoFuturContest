#include <nds.h>
#include "Capitalist_game.hpp"
#include "../assets/soviet.h"
#include "../assets/bar.h"

#include <maxmod9.h>    // Maxmod definitions for ARM9
#include "../assets/soundbank.h"
#include "../assets/soundbank_bin.h"  // Soundbank definitions

#define SOVIET_SPRITE (BAR_SPRITE + BAR_SPRITE_COUNT)
#define SOVIET_TILE (BAR_TILE + BAR_TILE_COUNT*5)
#define SOVIET_TILE_COUNT (8*8)
#define SOVIET_TILE_SIZE (SOVIET_TILE_COUNT*32)

Capitalist Capitalist::capitalist;
mm_sfxhand sfxhandle_punch;

void Capitalist::init()
{
	init_timer(2, 0);
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

	writeTimed("\x1b[10;10HTabasse!", 1000);

	bgHide(1);
}

void Capitalist::update()
{
	// timeout
	Game::update();

	if (!finished)
	{
		if (keysDown() & KEY_TOUCH)
		{
			touchPosition touch;
			touchRead(&touch);

			SpriteEntry* soviet = oamMain.oamMemory + SOVIET_SPRITE;

			if (	touch.px > soviet->x && touch.px < soviet->x + 64 &&
				touch.py > soviet->y && touch.py < soviet->y + 64)
			{
				// play punch sfx with random pitch
				sfxhandle_punch = mmEffect( SFX_PUNCH );
				mmEffectRate( sfxhandle_punch, 1024 + (rand() % 256 - 128) );

				++counter;

				if (counter >= 3)
				{
					mmEffect( SFX_OWWW );
					game_end(true);
				}
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
