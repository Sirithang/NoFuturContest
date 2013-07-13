#ifndef GAME_HUB_HPP
#define GAME_HUB_HPP

#include "Game.hpp"

#define NUM_OBSTACLE 5

class GameHub : public Game
{
	public:
		unsigned short player_life;

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

		struct Obstacle
		{
			short position;
			unsigned char type;
			bool success;
			bool active;
		};
		
		Obstacle obstacles[NUM_OBSTACLE];
		unsigned char current_obstacle;
		short next_obstacle_frame;

		enum PlayerAnim
		{
			RUN,
			JUMP,
			FALL,
		};
		PlayerAnim anim;
		unsigned short actual_frame;

		void new_obstacle();
};

#endif

