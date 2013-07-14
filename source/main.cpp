#include <nds.h>

#include "Game.hpp"
#include "GameHub.hpp"

#include <maxmod9.h>    // Maxmod definitions for ARM9
#include "../assets/soundbank.h"
#include "../assets/soundbank_bin.h"  // Soundbank definitions

int main()
{
	lcdMainOnBottom();

	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankB(VRAM_B_MAIN_SPRITE);
	vramSetBankC(VRAM_C_SUB_BG);
	vramSetBankD(VRAM_D_SUB_SPRITE);

	// Use this if you have the soundbank loaded into memory
    mmInitDefaultMem( (mm_addr)soundbank_bin );

	// SFX
	mmLoadEffect( SFX_FALL );
	mmLoadEffect( SFX_HURRY_UP );
	mmLoadEffect( SFX_JUMP );
	mmLoadEffect( SFX_STEP_LEFT );
	mmLoadEffect( SFX_STEP_RIGHT );
	mmLoadEffect( SFX_TOUCH );

	// MUSIC
	mmLoad( MOD_SENOR_ZORRO_ZA_RABOTU );
	mmSetModuleVolume( 512 );	// = 1/2
	mmSetModuleTempo( 820 );	// =~ 74.5 bpm
	mmStart( MOD_SENOR_ZORRO_ZA_RABOTU, MM_PLAY_LOOP );
		
	while (true)
	{
		// setup title screen

		unsigned int time = 0;

		// title screen loop
		do
		{
			swiWaitForVBlank();
			++time;
			scanKeys();
		} while (!(keysDown() & (KEY_START | KEY_TOUCH)));

		srand(time);

		// setup game
		videoSetMode(MODE_0_2D);
		videoSetModeSub(MODE_0_2D);

		// initialize the hub, set it at the current screen
		GameHub::hub.GameHub::init();

		unsigned int frame_number = 0;

		// game loop
		while (true)
		{
			scanKeys();
	
			bool update_top = !Game::is_game_playing() || (frame_number & 0x3) == 0;

			if (update_top)
			{
				// update the top screen
				GameHub::hub.update_top();
			}
		
			// update the bottom screen
			Game::current->update();

			// Wait for the vertical blank interrupt
			// before we can update the VRAM
			swiWaitForVBlank();

			// draw both screen
			if (update_top)
			{
				GameHub::hub.draw_top();
			}

			Game::current->draw();

			++frame_number;
		}

		// setup score screen
		
		// score screen loop
		do
		{
			swiWaitForVBlank();
			scanKeys();
		} while (!(keysDown() & (KEY_START | KEY_TOUCH)));
	}
}
