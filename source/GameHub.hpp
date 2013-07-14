#ifndef GAME_HUB_HPP
#define GAME_HUB_HPP

#include "Game.hpp"
#include "Usine_map.hpp"

#define NUM_OBSTACLE 5

extern UsineMap gUsine;

class GameHub : public Game
{
	public:
		enum State
		{
			PLAYING,
			WIN,
			LOSE,
		};

		State state;
		unsigned short player_life;
		unsigned short remaining_obstacles;

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

		u8 current_level;
		u8 current_level_obstacle_count;
		u8 speed; // over 16 to get the actual speed

		struct Obstacle
		{
			short position;
			unsigned char type;
			bool success;
			bool active;
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

