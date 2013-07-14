#ifndef AIM_HPP
#define AIM_HPP

#include "Game.hpp"

#define NUM_SOVIET 8

class Aim : public Game
{
	public:
		virtual void init();
		virtual void update();
		virtual void draw();

		static Aim aim;

	private:
		Aim() {}

	unsigned char frame[NUM_SOVIET + 1];
	unsigned char anim[NUM_SOVIET + 1];

	unsigned char capitalist_sprite;
};

#endif

