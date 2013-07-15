#include <nds.h>
#include <math.h>
#include <stdio.h>
#include "Game.hpp"
#include "GameHub.hpp"

#include "Capitalist_game.hpp"
#include "Dalton_game.hpp"
#include "Aim_game.hpp"
#include "Statue_game.hpp"
#include "Goulag_game.hpp"
#include "Couronne_game.hpp"

#include "../assets/bar.h"
#include "../assets/font.h"

#include <maxmod9.h>    // Maxmod definitions for ARM9
#include "../assets/soundbank.h"
#include "../assets/soundbank_bin.h"  // Soundbank definitions

#define BAR_X ((256 - BAR_LENGTH*16)/2)
#define BAR_Y 10

Game* Game::current;
Game* const Game::games[] = { &Capitalist::capitalist, &Dalton::dalton, &Aim::aim, &Statue::statue, &Couronne::couronne, &Goulag::goulag };
const int Game::game_count = sizeof(Game::games)/sizeof(Game*);

mm_sfxhand sfxhandle_hurry_up;

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

	//init console to write stuff and all
	console = consoleInit(0,0, BgType_Text4bpp, BgSize_T_256x256, 0, 20, true, false);

	ConsoleFont font;

	font.gfx = (u16*)fontTiles;
	font.pal = (u16*)fontPal;
	font.numChars = 95;
	font.numColors =  fontPalLen / 2;
	font.bpp = 4;
	font.asciiOffset = 32;
	font.convertSingleColor = false;
	
	consoleSetFont(console, &font);
	
	finished = 0;
	frameSinceFinished = 0;
}

void Game::update()
{
	if(finished)
	{
		frameSinceFinished--;

		if(frameSinceFinished == 0)
		{
			cleanup();
			timerStop(0);
			if (success)
			{
				GameHub::hub.minigame_success();
			}
			GameHub::hub.resume();
			current = &GameHub::hub;

			mmSetModuleVolume( 768 );	// = 3/4
			mmEffectCancel(sfxhandle_hurry_up);
		}

		return;
	}

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
	bgHide(gUsine.bg);
	bgHide(gUsine.bg2);

	current = games[game];
	current->init();

	mmSetModuleVolume( 256 );	// = 1/4
	sfxhandle_hurry_up = mmEffect( SFX_HURRY_UP );
}

void Game::game_end(bool success)
{
	current->success = success;
	current->finished = 1;
	current->frameSinceFinished = 30;

	timerStop(0);

	if(success)
	{
		current->writeTimed(current->winSentence, 500);
		mmJingle( MOD_JINGLE_WIN );
	}
	else
	{
		current->writeTimed(current->looseSentence, 500);
		mmJingle( MOD_JINGLE_LOSE );
	}
}

bool Game::is_game_playing()
{
	return current != &GameHub::hub;
}

//--------------------------------------

int currentNbCalls = 0;
int nbCallNeeded = 0;

void timerCallBack()
{
	currentNbCalls++;
	if(currentNbCalls >= nbCallNeeded)
	{
		consoleClear();
		timerStop(0);
	}
}

void Game::writeTimed(const char* message, int milliSeconds)
{
	consoleClear();
	timerStop(0);

	currentNbCalls = 0;
	nbCallNeeded = milliSeconds;

	iprintf(message);

	timerStart(0, ClockDivider_1024, TIMER_FREQ_1024(1000), timerCallBack);
}
