#include "GameHub.hpp"
#include <nds.h>

GameHub GameHub::hub;

void GameHub::init()
{
	// init the gameplay variables
	player_life = 100;
	obstacle_type = 0; // todo : RNG
	obstacle_position = 300;
	game_succeed = false;

	// init the sub screen
	oamClear(&oamSub, 0, 0);
	oamEnable(&oamSub);
	oamInit(&oamSub, SpriteMapping_1D_32, false);

	// init the main screen
	resume();

	// set as current
	Game::current = this;
}

void GameHub::resume()
{
	// setup the graphic engine,
	// restore background(s)
}

void GameHub::update()
{
	// one frame forward

	
}

void GameHub::draw()
{
	// update the main screen oam
	oamUpdate(&oamMain);
}

void GameHub::update_top()
{
	if (obstacle_position > -100)
	{
		--obstacle_position;
	}
	else
	{
		// throw in another obstacle
		obstacle_type = 0; // todo : RNG
		obstacle_position = 300;
		game_succeed = false;
	}

	// player
	oamSet(	&oamSub, 0, /*x*/0, /*y*/0, /*priority*/0, /*palette*/0,
		SpriteSize_32x64, SpriteColorFormat_16Color, oamGetGfxPtr(&oamSub, /*tile index*/0),
		0, false, false, false, false, false);

	// obstacle
	oamSet(	&oamSub, 0, /*x*/obstacle_position, /*y*/0, /*priority*/0, /*palette*/0,
		SpriteSize_32x32/*?*/, SpriteColorFormat_16Color, oamGetGfxPtr(&oamSub, /*tile index*/32),
		0, false, false, false, false, false);
}

void GameHub::draw_top()
{
	// animate tiles

	// update the OAM
	oamUpdate(&oamSub);
}
