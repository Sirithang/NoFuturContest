#include <nds.h>

#include "Game.hpp"
#include "GameHub.hpp"

int main()
{
	powerOn(POWER_ALL_2D);
	// Enable interrupts.
	irqInit();
	irqEnable(IRQ_VBLANK);

	// initialize the hub, set it at the current screen
	GameHub::hub.GameHub::init();

	unsigned int frame_number = 0;

	while (true)
	{
		bool update_top = !Game::is_game_playing() || (frame_number >> 2) == 0;

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
