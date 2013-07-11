#ifndef GAME_HPP
#define GAME_HPP

class Game
{
	public:
		virtual void init() = 0;
		virtual void update();
		virtual void draw() = 0;
		
		static void start_game(int game);
		static void game_end(bool success);
		static bool is_game_playing();

		static const int game_count;
		static Game* const games[];
		static Game* current;


	protected:
		void init_timer(unsigned char sec, unsigned char frame);

		// counter for whatever we want
		unsigned int counter;

	private:
		// number timer
		unsigned char timer_sec;
		unsigned char timer_frame; // 1/60th sec
};

#endif
