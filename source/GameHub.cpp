#include "../assets/player.h"
#include "../assets/obstacle.h"

#include "GameHub.hpp"
#include <nds.h>

GameHub GameHub::hub;

#define PLAYER_SPRITE 0
#define PLAYER_TILE 0
#define PLAYER_TILE_COUNT (4*8)
#define PLAYER_TILE_SIZE (PLAYER_TILE_COUNT*32)

#define OBSTACLE_SPRITE (PLAYER_SPRITE + 1)
#define OBSTACLE_TILE (PLAYER_TILE + PLAYER_TILE_COUNT)
#define OBSTACLE_TILE_COUNT (4*4)
#define OBSTACLE_TILE_SIZE (OBSTACLE_TILE_COUNT*32)

void GameHub::init()
{
	frame_counter = 0;

	// init the sub screen
	vramSetBankC(VRAM_C_SUB_BG);
	vramSetBankD(VRAM_D_SUB_SPRITE);

	// setup background

	// init oam
	oamClear(&oamSub, 0, 0);
	oamEnable(&oamSub);
	oamInit(&oamSub, SpriteMapping_1D_32, false);

	// load player's first frame
	dmaCopy(playerTiles, oamGetGfxPtr(&oamSub, PLAYER_TILE), PLAYER_TILE_SIZE);

	// init the gameplay variables
	player_life = 100;
	obstacle_type = 1;
	new_obstacle();

	// init the main screen
	resume();

	// set as current
	Game::current = this;
}

void GameHub::resume()
{
	// setup the graphic engine,
	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankB(VRAM_B_MAIN_SPRITE);

	// restore background(s)

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
	//Game::start_game(obstacle_type);
}

void GameHub::draw()
{
	// animate

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
	oamSet(	&oamSub, PLAYER_SPRITE, /*x*/0, /*y*/0, /*priority*/0, /*palette*/0,
		SpriteSize_32x64, SpriteColorFormat_16Color, oamGetGfxPtr(&oamSub, PLAYER_TILE),
		0, false, false, false, false, false);

	// obstacle
	oamSet(	&oamSub, OBSTACLE_SPRITE, /*x*/obstacle_position, /*y*/0, /*priority*/0, /*palette*/0,
		SpriteSize_32x32/*?*/, SpriteColorFormat_16Color, oamGetGfxPtr(&oamSub, OBSTACLE_TILE),
		0, false, false, false, false, false);
}

void GameHub::draw_top()
{
	// animate tiles
	if ((frame_counter & 0xf) == 0)
	{
		unsigned char frame = (frame_counter >> 4) & 0x3;
		dmaCopy(playerTiles + PLAYER_TILE_SIZE*frame, oamGetGfxPtr(&oamSub, PLAYER_TILE), PLAYER_TILE_SIZE);
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
