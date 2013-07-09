#ifndef GAME_HUB_HPP
#define GAME_HUB_HPP

#include "Game.hpp"

class GameHub : public Game
{
	private:
		GameHub() {}
		~GameHub() {}

	public:
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
};

#endif

