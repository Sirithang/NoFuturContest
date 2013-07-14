#include <nds.h>
#include "Dalton_game.hpp"
#include "../assets/buildings.h"
#include "../assets/bar.h"

#include <maxmod9.h>    // Maxmod definitions for ARM9
#include "../assets/soundbank.h"
#include "../assets/soundbank_bin.h"  // Soundbank definitions

#define DALTON_SPRITE (BAR_SPRITE + BAR_SPRITE_COUNT)
#define DALTON_TILE (BAR_TILE + BAR_TILE_COUNT*BAR_LENGTH)
#define DALTON_TILE_COUNT (4*8)
#define DALTON_TILE_SIZE (DALTON_TILE_COUNT*32)

Dalton Dalton::dalton;

void Dalton::init()
{
	init_timer(3, 0);
	counter = 0;

	oamClear(&oamMain, 0, 0);
	oamEnable(&oamMain);
	oamInit(&oamMain, SpriteMapping_1D_32, false);

	dmaCopy(buildingsTiles, oamGetGfxPtr(&oamMain, DALTON_TILE), sizeof(buildingsTiles));
	dmaCopy(buildingsPal, SPRITE_PALETTE, sizeof(buildingsPal));
	
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

	//bgHide(0);
	//bgHide(1);
	//bgHide(2);
	//bgHide(3);

	Game::init();

	writeTimed("\x1b[10;10HDu plus petit\nau plus grand !", 1000);
}

void Dalton::update()
{
	// timeout
	Game::update();

	if (keysDown() & KEY_TOUCH)
	{
		touchPosition touch;
		touchRead(&touch);

		if (	touch.px >= 64 + 32*positions[counter] && touch.px <= 64 + 32*(positions[counter] + 1) &&
			touch.py >= 64 && touch.py <= 128)
		{
			++counter;

			mmEffect( SFX_TOUCH );

			if (counter > 3)
			{
				game_end(true);
			}
		}
	}
	
	for (unsigned int i = 0; i < counter; ++i)
	{
		oamSet(	&oamMain, DALTON_SPRITE + positions[i], 64 + 32*positions[i], 64, /*priority*/1, /*palette*/1,
			SpriteSize_32x64, SpriteColorFormat_16Color, oamGetGfxPtr(&oamMain, DALTON_TILE + i*DALTON_TILE_COUNT),
			-1, false, false, false, false, false);
	}
	for (unsigned int i = counter; i < 4; ++i)
	{
		oamSet(	&oamMain, DALTON_SPRITE + positions[i], 64 + 32*positions[i], 64, /*priority*/1, /*palette*/0,
			SpriteSize_32x64, SpriteColorFormat_16Color, oamGetGfxPtr(&oamMain, DALTON_TILE + i*DALTON_TILE_COUNT),
			-1, false, false, false, false, false);
	}
}

void Dalton::draw()
{
	oamUpdate(&oamMain);
}
