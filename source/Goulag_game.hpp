#ifndef GOULAG_HPP
#define GOULAG_HPP

#include <nds.h>
#include "Game.hpp"

#define NUM_CAPITALIST 3

class Goulag : public Game
{
	public:
		virtual void init();
		virtual void cleanup();
		virtual void update();
		virtual void draw();

		static Goulag goulag;

	private:
		Goulag() {}

		bool checkin(const SpriteEntry * capitalist);
	
	unsigned char bg;

	unsigned char frame[NUM_CAPITALIST];
	unsigned char anim[NUM_CAPITALIST];

	unsigned char selected_capitalist;
	unsigned char select_x;
	unsigned char select_y;
};

#endif

