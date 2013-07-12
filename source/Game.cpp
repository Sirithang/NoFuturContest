#include <nds.h>
#include <math.h>
#include "Game.hpp"
#include "GameHub.hpp"
#include "Capitalist_game.hpp"
#include "../assets/bar.h"

#define BAR_X ((256 - BAR_LENGTH*16)/2)
#define BAR_Y 10

Game* Game::current;
Game* const Game::games[] = { &Capitalist::capitalist };
const int Game::game_count = sizeof(Game::games)/sizeof(Game*);

void Game::init_timer(unsigned char sec, unsigned char frame)
{
	timer_remaining_sec = timer_sec = sec;
	timer_remaining_frame = timer_frame = frame;
	timer_remaining_total_frame = timer_total_frame = 60*sec + frame;
}

void Game::init()
{
	dmaCopy(barTiles, oamGetGfxPtr(&oamMain, BAR_TILE), sizeof(barTiles));
	dmaCopy(barPal, SPRITE_PALETTE + 240, sizeof(barPal));

	oamSet(	&oamMain, BAR_SPRITE, BAR_X, BAR_Y, /*priority*/0, /*palette*/15,
		SpriteSize_16x16, SpriteColorFormat_16Color, oamGetGfxPtr(&oamMain, BAR_TILE + 2*BAR_TILE_COUNT),
		-1, false, false, false, false, false);

	for (int i = 1; i < BAR_LENGTH - 1; ++i)
	{
		oamSet(	&oamMain, BAR_SPRITE + i, BAR_X + 16*i, BAR_Y, /*priority*/0, /*palette*/15,
			SpriteSize_16x16, SpriteColorFormat_16Color, oamGetGfxPtr(&oamMain, BAR_TILE + 3*BAR_TILE_COUNT),
			-1, false, false, false, false, false);
	}

	oamSet(	&oamMain, BAR_SPRITE + BAR_LENGTH - 1, BAR_X + 16*(BAR_LENGTH - 1), BAR_Y, /*priority*/0, /*palette*/15,
		SpriteSize_16x16, SpriteColorFormat_16Color, oamGetGfxPtr(&oamMain, BAR_TILE + 4*BAR_TILE_COUNT),
		-1, false, false, false, false, false);
	
}

void Game::update()
{
	if (timer_remaining_total_frame == 0)
	{
		game_end(false);
	}
	else
	{
		--timer_remaining_total_frame;

		if (timer_frame == 0)
		{
			--timer_remaining_sec;
			timer_remaining_frame = 59;
		}
		else
		{
			--timer_remaining_frame;
		}

		// now, display timer bar !
		unsigned short visible_pixel_count = timer_remaining_total_frame*BAR_SIZE/timer_total_frame;
		unsigned short visible_tile_count = div32(visible_pixel_count + 2, 16);
		unsigned short half_tile_pixel_count = mod32(visible_pixel_count + 2, 16);
		if (half_tile_pixel_count == 0)
		{
			--visible_tile_count;
			half_tile_pixel_count = 16;
		}

		for (int i = 0; i < visible_tile_count; ++i)
		{
			oamSet(	&oamMain, BAR_SPRITE + BAR_LENGTH + i, BAR_X + 16*i, BAR_Y, /*priority*/0, /*palette*/15,
				SpriteSize_16x16, SpriteColorFormat_16Color, oamGetGfxPtr(&oamMain, BAR_TILE + BAR_TILE_COUNT),
				-1, false, false, false, false, false);
		}

		oamAffineTransformation(&oamMain, 0, 256*16/half_tile_pixel_count, 0, 0, 256);
		oamSet(	&oamMain, BAR_SPRITE + BAR_LENGTH + visible_tile_count, BAR_X + 16*visible_tile_count + half_tile_pixel_count/2 - 8, BAR_Y, 0, 15,
			SpriteSize_16x16, SpriteColorFormat_16Color, oamGetGfxPtr(&oamMain, BAR_TILE + BAR_TILE_COUNT),
			0, false, false, false, false, false);

		for (int i = visible_tile_count + 1; i < BAR_LENGTH; ++i)
		{
			oamClearSprite(&oamMain, BAR_SPRITE + BAR_LENGTH + i);
		}
	}
}

void Game::start_game(int game)
{
	current = games[game];
	current->init();
}

void Game::game_end(bool success)
{
	GameHub::hub.game_succeed = true;
	GameHub::hub.resume();

	current = &GameHub::hub;
}

bool Game::is_game_playing()
{
	return current != &GameHub::hub;
}
