#ifndef GAME_HUB_HPP
#define GAME_HUB_HPP

#include "Game.hpp"

class GameHub : public Game
{
	public:
		unsigned char player_frame;
		unsigned short player_life;

		short obstacle_position;
		unsigned char obstacle_type;
		bool game_succeed;

		virtual void init();
		void resume();
		virtual void update();
		virtual void draw();

		void update_top();
		void draw_top();
		
		static GameHub hub;

	private:
		GameHub() {}
		~GameHub() {}

		unsigned char frame_counter;
		void new_obstacle();
};

#endif

