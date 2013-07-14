#include "../assets/player.h"
#include "../assets/zorro_run.h"
#include "../assets/zorro_jump.h"
#include "../assets/zorro_fall.h"
#include "../assets/obstacle.h"
#include "../assets/symbole.h"

#include "../assets/background_haut_l.h"
#include "../assets/background_haut_r.h"

#include "GameHub.hpp"
#include "Objet.hpp"
#include "Usine_map.hpp"

#include <nds.h>
#include <math.h>

#include <maxmod9.h>    // Maxmod definitions for ARM9
#include "../assets/soundbank.h"
#include "../assets/soundbank_bin.h"  // Soundbank definitions

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
#define ICONE_SPRITE (PLAYER_SPRITE + 1)

Object playerObj;

UsineMap gUsine;
int bg;

void GameHub::init()
{
	frame_counter = 0;

	// *******************
	// init the sub screen
	// *******************

	// setup background
	BG_PALETTE_SUB[0] = 0;

	bg = bgInitSub(3, BgType_Text8bpp, BgSize_T_512x256, 0, 1);
	dmaCopy(background_haut_lTiles, bgGetGfxPtr(bg), sizeof(background_haut_lTiles));
	dmaCopy(background_haut_lMap, bgGetMapPtr(bg), sizeof(background_haut_lMap));

	dmaCopy(background_haut_rTiles, bgGetGfxPtr(bg) + sizeof(background_haut_lTiles)/2, sizeof(background_haut_rTiles));
	//dmaCopy(background_haut_rMap, bgGetMapPtr(bg) + 32*32, sizeof(background_haut_rMap));

	dmaCopy(background_haut_lPal, BG_PALETTE_SUB, sizeof(background_haut_lPal));

	u16* ptr = bgGetMapPtr(bg);
	for(int i = 0; i < (768); ++i)
	{
		ptr[1024 + i] = 768+i;
	}

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
	dmaCopy(symbolePal, SPRITE_PALETTE_SUB + 16, 32);
	dmaCopy(symbolePal+32, SPRITE_PALETTE_SUB + 32, 32);


	// init the gameplay variables
	state = PLAYING;
	player_life = 5;
	remaining_obstacles = 20;

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

	current_level = 0;
	current_level_obstacle_count = 0;
	speed = 1;
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
		if(gUsine._machines[idx].obstacle == obstacles[current_obstacle].iconeType && gUsine._machines[idx].usable)
		{
			machine::startWork(gUsine._machines[idx]);

			minigame_obstacle = current_obstacle;
			Game::start_game(obstacles[minigame_obstacle].type);
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
		if(anim != FALL)
			obstacles[i].position -= speed;
		
		const int positionJump[3] = {48, 46, 46};

		if (obstacles[i].position < -32)
		{
			obstacles[i].active = false;
		}
		else if (obstacles[i].position == positionJump[current_level])
		{
			if (obstacles[i].success)
			{
				--player_life;
				anim = FALL;
			}
			else
			{
				anim = JUMP;
			}
			actual_frame = 0;
			--obstacles[i].position;//to avoid infinite loop because obstacle is stuck at 48 when falling
		}
	}

	if(anim != FALL)
	{
		--next_obstacle_frame;
		if (next_obstacle_frame <= 0)
		{
			if(current_level < 2)
			{
				const int nbObstaclePerLevel[3] = {4,6,9};
				const int speeds[3] = {1,2,2};
				const int tempos[3] = {820, 922, 1024};

				current_level_obstacle_count++;

				if(current_level_obstacle_count == nbObstaclePerLevel[current_level])
				{
					current_level++;
					current_level_obstacle_count = 0;
					speed = speeds[current_level];
					mmSetModuleTempo( tempos[current_level] );
					mmPosition( 0 );
				}
			}

			// throw in another obstacle
			new_obstacle();
		}

		bgScroll(bg, +speed, 0);
		bgUpdate();
	}

	
	for (int i = 0; i < NUM_OBSTACLE; ++i)
	{
		//set icone
		int y = GROUND_HEIGHT + 32;
		if(anim == FALL && obstacles[i].position <= 48)
		{
			y += actual_frame;
			if(actual_frame == 8)
			{
				obstacles[i].active = false;
			}
		}

		oamSet(	&oamSub, OBSTACLE_SPRITE + i, obstacles[i].position, y, /*priority*/0, /*palette*/0,
			SpriteSize_32x32/*?*/, SpriteColorFormat_256Color, oamGetGfxPtr(&oamSub, OBSTACLE_TILE + obstacles[i].type*OBSTACLE_TILE_COUNT),
			-1, false, !obstacles[i].active, false, false, false);

		bool show = obstacles[i].active && (i == current_obstacle || obstacles[i].success);

		oamSet( &oamSub, ICONE_SPRITE + i, obstacles[i].position - 16, y - 16, 0, obstacles[i].success?2:1, SpriteSize_32x16, SpriteColorFormat_16Color, 
			oamGetGfxPtr(&oamMain, ICONE_TILE + (int)obstacles[i].iconeType * 8), 0, false,  !show, true, false, false);
	}

	int y = GROUND_HEIGHT;
	if (anim == JUMP)
	{
		// do not factorise ! we don't want to lose precision, so divisions MUST come last
		y -= (actual_frame*JUMP_HEIGHT - actual_frame*actual_frame*JUMP_HEIGHT/64)/(16*speed);
	}

	// player
	oamSet(	&oamSub, PLAYER_SPRITE, /*x*/0, y, /*priority*/0, /*palette*/0,
		SpriteSize_64x64, SpriteColorFormat_16Color, oamGetGfxPtr(&oamSub, playerObj.tile),
		-1, false, false, false, false, false);

	if (player_life <= 0)
	{
		state = WIN;
	}
	if (remaining_obstacles <= 0 && obstacles[current_obstacle].active == false)
	{
		state = LOSE;
	}
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
			if(player_frame == 0)		mmEffect( SFX_STEP_LEFT );
			else if(player_frame == 3)	mmEffect( SFX_STEP_RIGHT );
		}
		else if (anim == JUMP)
		{
			if (player_frame >= 8)
			{
				actual_frame = 0;
				player_frame = 0;
				anim = RUN;
				dmaCopy(zorro_runTiles, oamGetGfxPtr(&oamSub, PLAYER_TILE), PLAYER_TILE_SIZE);
				mmEffect( SFX_STEP_LEFT );
			}
			else
			{
				dmaCopy(zorro_jumpTiles + PLAYER_TILE_SIZE*player_frame, oamGetGfxPtr(&oamSub, PLAYER_TILE), PLAYER_TILE_SIZE);
				if(player_frame == 0) mmEffect( SFX_JUMP );
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
				mmEffect( SFX_STEP_LEFT );
			}
			else
			{
				dmaCopy(zorro_fallTiles + PLAYER_TILE_SIZE*player_frame, oamGetGfxPtr(&oamSub, PLAYER_TILE), PLAYER_TILE_SIZE);
				if(player_frame == 0) mmEffect( SFX_FALL );
			}
		}
	}

	// update the OAM
	oamUpdate(&oamSub);
}

void GameHub::new_obstacle()
{
	if (remaining_obstacles > 0)
	{
		--remaining_obstacles;

		++current_obstacle;
		if (current_obstacle >= NUM_OBSTACLE)
		{
			current_obstacle = 0;
		}

		obstacles[current_obstacle].type = mod32(rand(), Game::game_count);
		obstacles[current_obstacle].position = 256;
		obstacles[current_obstacle].success = false;
		obstacles[current_obstacle].active = true;

		obstacles[current_obstacle].iconeType = (ObstacleType)(rand()%MAX_OBSTACLE_TYPE);

		const int nbFrames[3]= {150, 90, 60};
		next_obstacle_frame = nbFrames[current_level];
	}
}

void GameHub::minigame_success()
{
	obstacles[minigame_obstacle].success = true;
}

