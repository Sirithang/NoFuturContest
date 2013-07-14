#include <nds.h>

#include "Game.hpp"
#include "GameHub.hpp"

#include <maxmod9.h>    // Maxmod definitions for ARM9

#include "../assets/titre.h"
#include "../assets/titre_bas.h"
#include "../assets/touche_pour_commencer.h"

#include "../assets/win.h"
#include "../assets/win_haut.h"
#include "../assets/lose.h"
#include "../assets/loser_haut.h"

#include "../assets/soundbank.h"
#include "../assets/soundbank_bin.h"  // Soundbank definitions

int main()
{
	lcdMainOnBottom();

	// setup game
	videoSetMode(MODE_0_2D);
	videoSetModeSub(MODE_0_2D);

	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankB(VRAM_B_MAIN_SPRITE);
	vramSetBankC(VRAM_C_SUB_BG);
	vramSetBankD(VRAM_D_SUB_SPRITE);

	mmInitDefaultMem( (mm_addr)soundbank_bin );

	// SFX
	mmLoadEffect( SFX_FALL );
	mmLoadEffect( SFX_HURRY_UP );
	mmLoadEffect( SFX_JUMP );
	mmLoadEffect( SFX_STEP_LEFT );
	mmLoadEffect( SFX_STEP_RIGHT );
	mmLoadEffect( SFX_TOUCH );

	// JINGLES
	mmLoad( MOD_JINGLE_WIN );
	mmLoad( MOD_JINGLE_LOSE );

	// MUSIC
	mmLoad( MOD_SENOR_ZORRO_ZA_RABOTU );
	mmSetModuleVolume(512);	// = 1/2
	mmSetModuleTempo(820);	// =~ 74.5 bpm
	mmStart( MOD_SENOR_ZORRO_ZA_RABOTU, MM_PLAY_LOOP );
	
	while (true)
	{
		// setup title screen
		int bg1 = bgInitSub(3, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
		dmaCopy(titreTiles, bgGetGfxPtr(bg1), sizeof(titreTiles));
		dmaCopy(titreMap, bgGetMapPtr(bg1), sizeof(titreMap));
		dmaCopy(titrePal, BG_PALETTE_SUB, sizeof(titrePal));
		
		int bg2 = bgInit(3, BgType_Text4bpp, BgSize_T_256x256, 0, 1);
		dmaCopy(titre_basTiles, bgGetGfxPtr(bg2), sizeof(titre_basTiles));
		dmaCopy(titre_basMap, bgGetMapPtr(bg2), sizeof(titre_basMap));
		dmaCopy(titre_basPal, BG_PALETTE, sizeof(titre_basPal));

		int bg3 = bgInit(2, BgType_Text4bpp, BgSize_T_256x256, 1, 2);
		dmaCopy(touche_pour_commencerTiles, bgGetGfxPtr(bg3), sizeof(touche_pour_commencerTiles));
		dmaCopy(touche_pour_commencerMap, bgGetMapPtr(bg3), sizeof(touche_pour_commencerMap));
		dmaCopy(touche_pour_commencerPal, BG_PALETTE + 16, sizeof(touche_pour_commencerPal));

		REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG3;

		unsigned int time = 0;
		unsigned int alpha = 0;

		// title screen loop
		do
		{
			if ((time % 2) == 0)
			{
				++alpha;
				if (alpha > 32)
				{
					alpha = 0;
				}

				if (alpha <= 16)
				{
					REG_BLDALPHA = alpha | ((16 - alpha) << 8);
				}
				else
				{
					REG_BLDALPHA = (32 - alpha) | ((alpha - 16) << 8);
				}
			}

			swiWaitForVBlank();
			++time;
			scanKeys();
		} while (!(keysDown() & (KEY_START | KEY_TOUCH)));

		bgHide(bg1);
		bgHide(bg2);
		bgHide(bg3);

		REG_BLDCNT = 0;

		srand(time);

		// initialize the hub, set it at the current screen
		GameHub::hub.GameHub::init();

		unsigned int frame_number = 0;

		// game loop
		while (GameHub::hub.state == GameHub::PLAYING)
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

		bgHide(gUsine.bg);
		bgHide(gUsine.bg2);

		// setup score screen
		oamDisable(&oamMain);
		oamDisable(&oamSub);
		
		if (GameHub::hub.state == GameHub::WIN)
		{
			bg1 = bgInitSub(3, BgType_Text4bpp, BgSize_T_256x256, 0, 1);
			dmaCopy(win_hautTiles, bgGetGfxPtr(bg1), sizeof(win_hautTiles));
			dmaCopy(win_hautMap, bgGetMapPtr(bg1), sizeof(win_hautMap));
			dmaCopy(win_hautPal, BG_PALETTE_SUB, sizeof(win_hautPal));
		
			bg2 = bgInit(3, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
			dmaCopy(winTiles, bgGetGfxPtr(bg2), sizeof(winTiles));
			dmaCopy(winMap, bgGetMapPtr(bg2), sizeof(winMap));
			dmaCopy(winPal, BG_PALETTE, sizeof(winPal));
		}
		else
		{
			bg1 = bgInitSub(3, BgType_Text4bpp, BgSize_T_256x256, 0, 1);
			dmaCopy(loser_hautTiles, bgGetGfxPtr(bg1), sizeof(loser_hautTiles));
			dmaCopy(loser_hautMap, bgGetMapPtr(bg1), sizeof(loser_hautMap));
			dmaCopy(loser_hautPal, BG_PALETTE_SUB, sizeof(loser_hautPal));
		
			bg2 = bgInit(3, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
			dmaCopy(loseTiles, bgGetGfxPtr(bg2), sizeof(loseTiles));
			dmaCopy(loseMap, bgGetMapPtr(bg2), sizeof(loseMap));
			dmaCopy(losePal, BG_PALETTE, sizeof(losePal));
		}

		// score screen loop
		do
		{
			swiWaitForVBlank();
			scanKeys();
		} while (!(keysDown() & (KEY_START | KEY_TOUCH)));

		bgHide(bg1);
		bgHide(bg2);
	}
}
