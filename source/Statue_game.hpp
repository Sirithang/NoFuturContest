#ifndef STATUE_HPP
#define STATUE_HPP

#include "Game.hpp"

class Statue : public Game
{
	public:
		virtual void init();
		virtual void update();
		virtual void draw();

		static Statue statue;

	private:
		Statue() : Game("\x1b[10;10HVictoire!", "\x1b[10;10HEchec!") {}
};

#endif
