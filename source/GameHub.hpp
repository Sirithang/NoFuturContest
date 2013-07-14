#ifndef GAME_HUB_HPP
#define GAME_HUB_HPP

#include "Game.hpp"
#include "Usine_map.hpp"

#define NUM_OBSTACLE 5

class GameHub : public Game
{
	public:
		unsigned short player_life;

		virtual void init();
		void resume();
		virtual void update();
		virtual void draw();

		void update_top();
		void draw_top();
		
		static GameHub hub;

		void minigame_success();

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
			bool can_change;//this is set to false if it's not posible to avoid the obstacle now.
			ObstacleType iconeType;
		};
		
		Obstacle obstacles[NUM_OBSTACLE];
		unsigned char minigame_obstacle;
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

