#ifndef CAPITALIST_HPP
#define CAPITALIST_HPP

#include "Game.hpp"

class Capitalist : public Game
{
	public:
		virtual void init();
		virtual void update();
		virtual void draw();

		static Capitalist capitalist;

	private:
		Capitalist() {}

		unsigned char frame;
		unsigned char anim;
};

#endif
