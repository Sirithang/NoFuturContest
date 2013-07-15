#include <nds.h>
#include "Couronne_game.hpp"
#include "../assets/couronne.h"
#include "../assets/tsar.h"
#include "../assets/bar.h"

#include <maxmod9.h>    // Maxmod definitions for ARM9
#include "../assets/soundbank.h"
#include "../assets/soundbank_bin.h"  // Soundbank definitions

#define COURONNE_SPRITE (BAR_SPRITE + BAR_SPRITE_COUNT)
#define COURONNE_TILE (BAR_TILE + BAR_TILE_COUNT*5)
#define COURONNE_TILE_COUNT (8*8)
#define COURONNE_TILE_SIZE (COURONNE_TILE_COUNT*32)

Couronne Couronne::couronne;
//mm_sfxhand sfxhandle_matrioshka;

void Couronne::init()
{
	init_timer(2, 0);
	counter = 0;

	

	oamClear(&oamMain, 0, 0);
	oamEnable(&oamMain);
	oamInit(&oamMain, SpriteMapping_1D_32, false);

	dmaCopy(couronneTiles, oamGetGfxPtr(&oamMain, COURONNE_TILE), sizeof(couronneTiles));
	dmaCopy(couronnePal, SPRITE_PALETTE+16, sizeof(couronnePal));
	
	int dir[2]= {0,200};
	couronne_position.x = dir[rand()%2];
	couronne_position.y = 30;
	
	oamSet(	&oamMain, COURONNE_SPRITE, couronne_position.x, couronne_position.y, /*priority*/0, /*palette*/1,
		SpriteSize_64x64, SpriteColorFormat_16Color, oamGetGfxPtr(&oamMain, COURONNE_TILE),
		-1, false, false, false, false, false);

	touched = false;
	grabbed = false;

	Game::init();

	int bg = bgInit(3, BgType_Text4bpp, BgSize_T_256x256, 1, 2);
	bgSetPriority(bg, 3);
	dmaCopy(tsarTiles, bgGetGfxPtr(bg), sizeof(tsarTiles));
	dmaCopy(tsarPal, BG_PALETTE + 16, sizeof(tsarPal));
	//dmaCopy(tsarMap, bgGetMapPtr(bg), sizeof(tsarMap));

	u16* mapPtr = bgGetMapPtr(bg);
	u16* tsarPtr = (u16*)tsarMap;
	for(int i = 0; i < tsarMapLen/2; ++i)
	{
		mapPtr[i] = tsarPtr[i] | TILE_PALETTE(1);
	}

	writeTimed("\x1b[12;7HCouronne le tsar!", 1000);
}

void Couronne::update()
{
	// timeout
	Game::update();

	if (!finished)
	{
		touchPosition touch;

		if (keysCurrent() & KEY_TOUCH)
		{
			touchRead(&touch);

			//check touched the sprite
			if(!touched)
			{
				touched = true;

				if(touch.px >= couronne_position.x && touch.px <= couronne_position.x + 64 &&
					touch.py >= couronne_position.y && touch.py <= couronne_position.y + 64)
				{
					grabbed = true;
					last_pos.x = touch.px;
					last_pos.y = touch.py;
				}
			}
		}
		else
		{
			touched = false;
			grabbed = false;
		}

		if(grabbed)
		{
			Vec2i delta;
			delta.x = touch.px - last_pos.x;
			delta.y = touch.py - last_pos.y;

			couronne_position.x += delta.x;
			couronne_position.y += delta.y;

			last_pos.x = touch.px;
			last_pos.y = touch.py;
		}

		//check completion
		if(couronne_position.x > 256/2 - 60 && couronne_position.x < 256/2 - 20 &&
			couronne_position.y < 0)
		{
			Game::game_end(true);
		}


		oamSet(	&oamMain, COURONNE_SPRITE, couronne_position.x, couronne_position.y, /*priority*/0, /*palette*/1,
		SpriteSize_64x64, SpriteColorFormat_16Color, oamGetGfxPtr(&oamMain, COURONNE_TILE),
		-1, false, false, false, false, false);
	}
}

void Couronne::draw()
{
	oamUpdate(&oamMain);
}
