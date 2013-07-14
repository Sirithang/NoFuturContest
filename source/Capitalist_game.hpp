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
		Capitalist() : Game("\x1b[10;10HVictoire!", "\x1b[10;10HEchec!") {}

		// sprite's topleft position
		int capitalist_x;
		int capitalist_y;
};

#endif
