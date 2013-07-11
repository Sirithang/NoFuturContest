#include "../assets/player.h"
#include "../assets/zorro_spriteset.h"
#include "../assets/obstacle.h"
#include "../assets/test_usine_tilesets.h"

#include "GameHub.hpp"
#include "Objet.hpp"

#include <nds.h>
#include <stdlib.h>
#include <time.h>

GameHub GameHub::hub;

#define PLAYER_SPRITE 0
#define PLAYER_TILE 0
#define PLAYER_TILE_COUNT (8*8)
#define PLAYER_TILE_SIZE (PLAYER_TILE_COUNT*32)

#define OBSTACLE_SPRITE (PLAYER_SPRITE + 1)
#define OBSTACLE_TILE (PLAYER_TILE + PLAYER_TILE_COUNT)
#define OBSTACLE_TILE_COUNT (4*4)
#define OBSTACLE_TILE_SIZE (OBSTACLE_TILE_COUNT*32)

Object playerObj;

struct Usine
{
	int bg;
	u16 map[12*16];
} gUsine;

void GameHub::init()
{
	frame_counter = 0;

	// *******************
	// init the sub screen
	// *******************

	// setup background

	// init oam
	oamClear(&oamSub, 0, 0);
	oamEnable(&oamSub);
	oamInit(&oamSub, SpriteMapping_1D_32, false);

	object::setPos(playerObj, 40, 40);
	playerObj.sprite = PLAYER_SPRITE;
	playerObj.tile = PLAYER_TILE;
	playerObj.palette = 0;

	// load player's first frame
	player_frame = 0;
	dmaCopy(zorro_spritesetTiles, oamGetGfxPtr(&oamSub, PLAYER_TILE), PLAYER_TILE_SIZE);
	dmaCopy(zorro_spritesetPal, SPRITE_PALETTE_SUB, sizeof(zorro_spritesetPal));

	// init the gameplay variables
	player_life = 100;
	obstacle_type = 1;
	new_obstacle();

	// ********************
	// init the main screen
	// ********************

	for(int i = 0; i < 12*16; ++i)
		gUsine.map[i] = 0;

	srand(time(NULL));

	//create the procedural usine
	// place 5 machine anywhere
	for(int i = 0; i < 5; ++i)
	{
		u8 y = rand()%12;
		u8 x = rand()%16;

		gUsine.map[x*16 + y] = rand()%5;
	}

	resume();

	// set as current
	Game::current = this;
}

void GameHub::resume()
{
	// restore background(s)
	gUsine.bg = bgInit(0, BgType_Text8bpp, BgSize_T_256x256, 0,1);
	dmaCopy(test_usine_tilesetsTiles,bgGetGfxPtr(gUsine.bg), test_usine_tilesetsTilesLen);
	dmaCopy(test_usine_tilesetsPal, BG_PALETTE, sizeof(test_usine_tilesetsPal));

	u16* mapPtr = bgGetMapPtr(gUsine.bg);
	for(int i = 0; i < 24; ++i)
	{
		for(int j = 0; j < 32; ++j)
		{
			u32 idx = i * 32 + j;
			u8 x = i / 2;
			u8 y = j / 2;

			mapPtr[idx] = bgGetMapBase(gUsine.bg)+ ((gUsine.map[x*12 + y] == 0 ? 0 : 62)  * 4) + ((i%2)*2 + (j%2));
		}
	}

	// reinit oam
	oamClear(&oamMain, 0, 0);
	oamEnable(&oamMain);
	oamInit(&oamMain, SpriteMapping_1D_32, false);

	// restore OAM tiles

	// position sprites
}

void GameHub::update()
{
	// one frame forward

	// check if it is time to click minigame
	// and the correct game is clicked

	if (keysDown() & KEY_TOUCH)
	{
		touchPosition position;
		touchRead(&position);

		// todo : check position.px and position.py 
		{
			Game::start_game(0);
		}
	}

}

void GameHub::draw()
{
	// animate
	bgUpdate();
	// update the main screen oam
	oamUpdate(&oamMain);
}

void GameHub::update_top()
{
	if (obstacle_position > -50)
	{
		--obstacle_position;
	}
	else
	{
		// throw in another obstacle
		new_obstacle();
	}

	// player
	oamSet(	&oamSub, playerObj.sprite, /*x*/playerObj.x, /*y*/playerObj.y, /*priority*/0, /*palette*/playerObj.palette,
		SpriteSize_64x64, SpriteColorFormat_16Color, oamGetGfxPtr(&oamSub, playerObj.tile),
		0, false, false, false, false, false);

	// obstacle
	oamSet(	&oamSub, OBSTACLE_SPRITE, /*x*/obstacle_position, /*y*/0, /*priority*/0, /*palette*/0,
		SpriteSize_32x32/*?*/, SpriteColorFormat_16Color, oamGetGfxPtr(&oamSub, OBSTACLE_TILE),
		0, false, false, false, false, false);
}

void GameHub::draw_top()
{
	// animate tiles
	if ((frame_counter & 0x7) == 0)
	{
		player_frame = player_frame < 5 ? player_frame + 1 : 0;
		dmaCopy(zorro_spritesetTiles + PLAYER_TILE_SIZE*player_frame, oamGetGfxPtr(&oamSub, PLAYER_TILE), PLAYER_TILE_SIZE);
	}

	// update the OAM
	oamUpdate(&oamSub);

	++frame_counter;
}

void GameHub::new_obstacle()
{
	obstacle_type = obstacle_type == 1 ? 0 : 1; // todo : RNG
	obstacle_position = 270;
	game_succeed = false;

	// get the obstacle sprite
	dmaCopy(obstacleTiles + OBSTACLE_TILE_SIZE*obstacle_type, oamGetGfxPtr(&oamSub, OBSTACLE_TILE), OBSTACLE_TILE_SIZE);
}
