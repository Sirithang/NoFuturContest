#include "../assets/player.h"
#include "../assets/zorro_run.h"
#include "../assets/zorro_jump.h"
#include "../assets/zorro_fall.h"
#include "../assets/obstacle.h"
#include "../assets/symbole.h"

#include "../assets/background_haut.h"

#include "../assets/vie.h"
#include "../assets/vie_couleurs.h"

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
#define ICONE_TILE_COUNT (4*2*8)
#define ICONE_SPRITE (PLAYER_SPRITE + 1)

//to fucking be sur to not overlap
#define VIE_SPRITE 60
#define VIE_TILE (ICONE_TILE+ICONE_TILE_COUNT)
#define VIE_COLOR_TILE (VIE_TILE+ 4*2*5)

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
	dmaCopy(background_hautMap, bgGetMapPtr(bg), sizeof(background_hautMap));
	dmaCopy(background_hautTiles, bgGetGfxPtr(bg), sizeof(background_hautTiles));

	dmaCopy(background_hautPal, BG_PALETTE_SUB, sizeof(background_hautPal));

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

	//load life bar
	dmaCopy(vieTiles, oamGetGfxPtr(&oamSub, VIE_TILE), sizeof(vieTiles));
	dmaCopy(viePal, SPRITE_PALETTE_SUB + 48, sizeof(viePal));

	dmaCopy(vie_couleursTiles, oamGetGfxPtr(&oamSub, VIE_COLOR_TILE), sizeof(vie_couleursTiles));
	dmaCopy(vie_couleursPal, SPRITE_PALETTE_SUB + 64, sizeof(vie_couleursPal));

	// init the gameplay variables
	player_life = 0;
	state = PLAYING;

	for (int i = 0; i < NUM_OBSTACLE; ++i)
	{
		obstacles[i].active = false;
	}

	current_level = 0;
	current_level_obstacle_count = 0;
	speed = 1;
	current_obstacle = 0;
	new_obstacle();

	// ********************
	// init the main screen
	// ********************

	usine::init(gUsine);

	resume();

	// set as current
	Game::current = this;

	nextBgScroll = 0;
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
		if (obstacles[i].active)
		{
			if(anim != FALL)
			{
				obstacles[i].position -= speed;
			}

			const int positionJump[3] = {48, 46, 46};

			if (obstacles[i].position < -32)
			{
				obstacles[i].active = false;
			}
			else if (obstacles[i].position == positionJump[current_level])
			{
				anim = obstacles[i].success ? FALL : JUMP;
				actual_frame = 0;
				--obstacles[i].position;//to avoid infinite loop because obstacle is stuck at 48 when falling

				if(!Game::is_game_playing())
				{
					if(anim == FALL)
					{
						if(player_life < 10)
							player_life += 1;
					}
					else
					{
						if(player_life > 0)
							player_life -= 1;
					}
				}
			}
		}
	}

	if(anim != FALL)
	{
		--next_obstacle_frame;
		if (next_obstacle_frame <= 0)
		{
			const int nbObstaclePerLevel[3] = {4,6,9};
			const int speed_lev[3] = {1, 2,2};

			current_level_obstacle_count++;

			if(current_level_obstacle_count == nbObstaclePerLevel[current_level])
			{
				if(current_level < 2)
				{
					current_level++;
					current_level_obstacle_count = 0;
					speed = speed_lev[current_level];
				}
				else
				{
					if(player_life >= 6)
					{
						state = WIN;
					}
					else
					{
						state = LOSE;
					}
				}
			}

			new_obstacle();
		}

		if(nextBgScroll == 0)
		{
			bgScroll(bg, +speed, 0);
			nextBgScroll = 30;
		}
		
		nextBgScroll--;

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


	//draw lifebar

	for(int i = 0; i < 5; ++i)
	{
		oamSet(&oamSub, VIE_SPRITE+i, SCREEN_WIDTH/2 - 160/2 + i*32, 10, 0, 3, SpriteSize_32x16, SpriteColorFormat_16Color, oamGetGfxPtr(&oamSub, VIE_TILE + i*8), 0, false,  false, false, false, false);
	}

	for(int i = 0; i < 10; ++i)
	{
		bool show = player_life > i;
		oamSet(&oamSub, VIE_SPRITE+5+i, SCREEN_WIDTH/2 - 160/2 + i*16, 10, 0, 4, SpriteSize_16x16, SpriteColorFormat_16Color, oamGetGfxPtr(&oamSub, VIE_COLOR_TILE + i*4), 0, false, !show, false, false, false);
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

	const int nbFrames[3]= {150, 100, 80};
	next_obstacle_frame = nbFrames[current_level];
}

void GameHub::minigame_success()
{
	obstacles[minigame_obstacle].success = true;
}

