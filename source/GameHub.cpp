#include "../assets/player.h"
#include "../assets/zorro_run.h"
#include "../assets/zorro_jump.h"
#include "../assets/zorro_fall.h"
#include "../assets/obstacle.h"
#include "../assets/symbole.h"

#include "GameHub.hpp"
#include "Objet.hpp"
#include "Usine_map.hpp"

#include <nds.h>

GameHub GameHub::hub;

#define GROUND_HEIGHT 100
#define JUMP_HEIGHT 56

#define OBSTACLE_COUNT 10
#define OBSTACLE_SPRITE 0
#define OBSTACLE_TILE 0
#define OBSTACLE_TILE_COUNT (4*4*2)
#define OBSTACLE_TILE_SIZE (OBSTACLE_TILE_COUNT*32)

#define PLAYER_SPRITE (OBSTACLE_SPRITE + NUM_OBSTACLE)
#define PLAYER_TILE (OBSTACLE_TILE + OBSTACLE_COUNT*OBSTACLE_TILE_COUNT)
#define PLAYER_TILE_COUNT (8*8)
#define PLAYER_TILE_SIZE (PLAYER_TILE_COUNT*32)

#define ICONE_SPRITE (PLAYER_SPRITE + 1)
#define ICONE_TILE (PLAYER_TILE + PLAYER_TILE_COUNT)

Object playerObj;

UsineMap gUsine;

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
	actual_frame = 0;
	anim = RUN;
	dmaCopy(zorro_runTiles, oamGetGfxPtr(&oamSub, PLAYER_TILE), PLAYER_TILE_SIZE);
	dmaCopy(zorro_runPal, SPRITE_PALETTE_SUB, sizeof(zorro_runPal));

	// load obstacle tileset
	dmaCopy(obstacleTiles, oamGetGfxPtr(&oamSub, OBSTACLE_TILE), sizeof(obstacleTiles));
	dmaCopy(obstaclePal, SPRITE_PALETTE_SUB + 224, sizeof(obstaclePal));

	//load icone tileset
	dmaCopy(symboleTiles, oamGetGfxPtr(&oamSub, ICONE_TILE), sizeof(symboleTiles));
	dmaCopy(symbolePal, SPRITE_PALETTE_SUB + 16, sizeof(symbolePal));

	// init the gameplay variables
	player_life = 100;

	for (int i = 0; i < NUM_OBSTACLE; ++i)
	{
		obstacles[i].active = false;
	}

	current_obstacle = 0;
	new_obstacle();

	// ********************
	// init the main screen
	// ********************

	usine::init(gUsine);

	resume();

	// set as current
	Game::current = this;
}

void GameHub::resume()
{
	// restore background(s)
	usine::restoreGraphics(gUsine);

	// reinit oam
	oamClear(&oamMain, 0, 0);
	oamEnable(&oamMain);
	oamInit(&oamMain, SpriteMapping_1D_32, false);

	// restore OAM tiles

	usine::restoreSprite(gUsine);

	// position sprites
}

void GameHub::update()
{
	// one frame forward
	usine::update(gUsine);

	// check if it is time to click minigame
	// and the correct game is clicked

	if (keysDown() & KEY_TOUCH)
	{
		touchPosition position;
		touchRead(&position);

		Vec2i usineCase = usine::getCase(gUsine, position);
		int idx = usineCase.x + usineCase.y * gUsine.w;
		if(gUsine._machines[idx].obstacle == obstacles[current_obstacle].iconeType)
		{
			gUsine._machines[idx].workerSpritesheetOffset = SPRITESHEET_WORK;

			minigame_obstacle = current_obstacle;
			Game::start_game(1);
		}
	}

}

void GameHub::draw()
{
	// update the main screen oam
	oamUpdate(&oamMain);
}

void GameHub::update_top()
{
	++actual_frame;

	for (int i = 0; i < NUM_OBSTACLE; ++i)
	{
		--obstacles[i].position;
		
		if (obstacles[i].position < -32)
		{
			obstacles[i].active = false;
		}
		else if (obstacles[i].position == 48)
		{
			anim = obstacles[i].success ? FALL : JUMP;
			actual_frame = 0;
		}
	}

	--next_obstacle_frame;
	if (next_obstacle_frame <= 0)
	{
		// throw in another obstacle
		new_obstacle();
	}

	
	for (int i = 0; i < NUM_OBSTACLE; ++i)
	{
		oamSet(	&oamSub, OBSTACLE_SPRITE + i, obstacles[i].position, GROUND_HEIGHT + 32, /*priority*/0, /*palette*/0,
			SpriteSize_32x32/*?*/, SpriteColorFormat_256Color, oamGetGfxPtr(&oamSub, OBSTACLE_TILE + obstacles[i].type*OBSTACLE_TILE_COUNT),
			-1, false, !obstacles[i].active, false, false, false);

		//set icone

		oamSet( &oamSub, PLAYER_SPRITE + i, obstacles[i].position + 32, GROUND_HEIGHT + 32, 0, 1, SpriteSize_32x16, SpriteColorFormat_16Color, 
			oamGetGfxPtr(&oamMain, ICONE_TILE + (int)obstacles[i].iconeType * 8), 0, false,  !obstacles[i].active, true, false, false);
	}

	int y = GROUND_HEIGHT;
	if (anim == JUMP)
	{
		// do not factorise ! we don't want to lose precision, so divisions MUST come last
		y -= (actual_frame*JUMP_HEIGHT - actual_frame*actual_frame*JUMP_HEIGHT/64)/16;
	}

	// player
	oamSet(	&oamSub, PLAYER_SPRITE, /*x*/0, y, /*priority*/0, /*palette*/0,
		SpriteSize_64x64, SpriteColorFormat_16Color, oamGetGfxPtr(&oamSub, playerObj.tile),
		-1, false, false, false, false, false);

}

void GameHub::draw_top()
{
	// animate tiles
	if ((actual_frame & 0x7) == 0)
	{
		int player_frame = actual_frame/8;

		if (anim == RUN)
		{
			if (player_frame >= 6)
			{
				actual_frame = 0;
				player_frame = 0;
			}
			dmaCopy(zorro_runTiles + PLAYER_TILE_SIZE*player_frame, oamGetGfxPtr(&oamSub, PLAYER_TILE), PLAYER_TILE_SIZE);
		}
		else if (anim == JUMP)
		{
			if (player_frame >= 8)
			{
				actual_frame = 0;
				player_frame = 0;
				anim = RUN;
				dmaCopy(zorro_runTiles, oamGetGfxPtr(&oamSub, PLAYER_TILE), PLAYER_TILE_SIZE);
			}
			else
			{
				dmaCopy(zorro_jumpTiles + PLAYER_TILE_SIZE*player_frame, oamGetGfxPtr(&oamSub, PLAYER_TILE), PLAYER_TILE_SIZE);
			}
		}
		else // FALL
		{
			if (player_frame >= 8)
			{
				actual_frame = 0;
				player_frame = 0;
				anim = RUN;
				dmaCopy(zorro_runTiles, oamGetGfxPtr(&oamSub, PLAYER_TILE), PLAYER_TILE_SIZE);
			}
			else
			{
				dmaCopy(zorro_fallTiles + PLAYER_TILE_SIZE*player_frame, oamGetGfxPtr(&oamSub, PLAYER_TILE), PLAYER_TILE_SIZE);
			}
		}
	}

	// update the OAM
	oamUpdate(&oamSub);
}

void GameHub::new_obstacle()
{
	unsigned char type = obstacles[current_obstacle].type + 1;

	++current_obstacle;
	if (current_obstacle >= NUM_OBSTACLE)
	{
		current_obstacle = 0;
	}

	obstacles[current_obstacle].type = type == OBSTACLE_COUNT ? (ObstacleType)0 : (ObstacleType)type; // todo : RNG
	obstacles[current_obstacle].position = 256;
	obstacles[current_obstacle].success = false;
	obstacles[current_obstacle].active = true;

	obstacles[current_obstacle].iconeType = (ObstacleType)(rand()%MAX_OBSTACLE_TYPE);

	next_obstacle_frame = 150;
}

void GameHub::minigame_success()
{
	obstacles[minigame_obstacle].success = true;
}

