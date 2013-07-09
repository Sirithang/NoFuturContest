#ifndef GAME_HPP
#define GAME_HPP

class Game
{
	public:
		virtual void init() = 0;
		virtual void update() = 0;
		virtual void draw() = 0;
		
		static void start_game(int game);
		static void game_end(bool success);
		static bool is_game_playing();

		static const int game_count;
		static Game* games[];
		static Game* current;
};

#endif
