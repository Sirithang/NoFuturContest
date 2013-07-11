#include <nds.h>

#include "Game.hpp"
#include "GameHub.hpp"

int main()
{
	videoSetMode(MODE_0_2D);
	videoSetModeSub(MODE_0_2D);

	lcdMainOnBottom();

	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankB(VRAM_B_MAIN_SPRITE);
	vramSetBankC(VRAM_C_SUB_BG);
	vramSetBankD(VRAM_D_SUB_SPRITE);
	
	// Enable interrupts.
	//irqInit();
	//irqEnable(IRQ_VBLANK);
	
	SPRITE_PALETTE[1] = RGB15(31,31,31);
	SPRITE_PALETTE_SUB[1] = RGB15(31,31,31);
	//SPRITE_PALETTE[1] = RGB15(0,0,0);

	// initialize the hub, set it at the current screen
	GameHub::hub.GameHub::init();

	unsigned int frame_number = 0;

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
}
