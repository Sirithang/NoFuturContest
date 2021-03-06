#ifndef GAME_HPP
#define GAME_HPP

#include <nds.h>

#define BAR_LENGTH 8
#define BAR_SIZE (BAR_LENGTH*16 - 4)
#define BAR_SPRITE 0
#define BAR_SPRITE_COUNT (BAR_LENGTH*2)
#define BAR_TILE 0
#define BAR_TILE_COUNT (2*2)
#define BAR_TILE_SIZE (BAR_TILE_COUNT*32)

class Game
{
	public:
		virtual void init();
		virtual void update();
		virtual void draw() = 0;
		virtual void cleanup() {}
		
		static void start_game(int game);
		static void game_end(bool success);
		static bool is_game_playing();

		static const int game_count;
		static Game* const games[];
		static Game* current;

	protected:
		void init_timer(unsigned char sec, unsigned char frame);

		void writeTimed(const char* message, int milliSeconds);

		// counter for whatever we want
		unsigned int counter;
		//console to write
		PrintConsole *console;

		const char* winSentence;
		const char* looseSentence;
		Game(const char* win = "\x1b[10;10HVictoire!", const char* loose = "\x1b[10;10HEchec!") : winSentence(win), looseSentence(loose){}

		u8 finished;

	private:
		// timer
		unsigned char timer_sec;
		unsigned char timer_frame; // 1/60th sec
		unsigned short timer_total_frame;

		unsigned char timer_remaining_sec;
		unsigned char timer_remaining_frame; // 1/60th sec
		unsigned short timer_remaining_total_frame;

		u8 success;
		u16 frameSinceFinished;
};

#endif
