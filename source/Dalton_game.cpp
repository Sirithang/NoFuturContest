#include <nds.h>
#include "Dalton_game.hpp"
#include "../assets/matriochka.h"
#include "../assets/bar.h"

#include <maxmod9.h>    // Maxmod definitions for ARM9
#include "../assets/soundbank.h"
#include "../assets/soundbank_bin.h"  // Soundbank definitions

#define DALTON_SPRITE (BAR_SPRITE + BAR_SPRITE_COUNT)
#define DALTON_TILE (BAR_TILE + BAR_TILE_COUNT*5)
#define DALTON_TILE_COUNT (4*8)
#define DALTON_TILE_SIZE (DALTON_TILE_COUNT*32)

Dalton Dalton::dalton;
mm_sfxhand sfxhandle_matrioshka;

void Dalton::init()
{
	init_timer(3, 0);
	counter = 0;

	oamClear(&oamMain, 0, 0);
	oamEnable(&oamMain);
	oamInit(&oamMain, SpriteMapping_1D_32, false);

	dmaCopy(matriochkaTiles, oamGetGfxPtr(&oamMain, DALTON_TILE), sizeof(matriochkaTiles));
	dmaCopy(matriochkaPal, SPRITE_PALETTE, sizeof(matriochkaPal));
	
	positions[0] = rand() & 0x3;
	do
	{
		positions[1] = rand() & 0x3;
	} while (positions[1] == positions[0]);

	unsigned char free[2];
	for (free[0] = 0; free[0] == positions[0] || free[0] == positions[1]; ++free[0]);
	for (free[1] = free[0] + 1; free[1] == positions[0] || free[1] == positions[1]; ++free[1]);

	positions[2] = rand() & 0x1;
	positions[3] = free[1 - positions[2]];
	positions[2] = free[positions[2]];

	for (int i = 0; i < 4; ++i)
	{
		oamSet(	&oamMain, DALTON_SPRITE + positions[i], 64 + 32*positions[i], 64, /*priority*/1, /*palette*/0,
			SpriteSize_32x64, SpriteColorFormat_16Color, oamGetGfxPtr(&oamMain, DALTON_TILE + i*DALTON_TILE_COUNT),
			-1, false, false, false, false, false);
	}

	Game::init();

	writeTimed("\x1b[11;6HDu plus petit\nau plus grand !", 1000);

	bgHide(1);
}

void Dalton::update()
{
	// timeout
	Game::update();

	if (!finished)
	{
		if (keysDown() & KEY_TOUCH)
		{
			touchPosition touch;
			touchRead(&touch);

			if (	touch.px >= 64 + 32*positions[counter] && touch.px <= 64 + 32*(positions[counter] + 1) &&
				touch.py >= 64 && touch.py <= 128)
			{
				(oamMain.oamMemory + DALTON_SPRITE + positions[counter])->palette = 1;

				// play SFX with added pitch
				sfxhandle_matrioshka = mmEffect( SFX_MATRIOSHKA );
				mmEffectRate( sfxhandle_matrioshka, 1024 + counter * 128);

				++counter;

				if (counter > 3)
				{
					game_end(true);
				}
			}
		}
	}
}

void Dalton::draw()
{
	oamUpdate(&oamMain);
}
