#ifndef DALTON_HPP
#define DALTON_HPP

#include "Game.hpp"

class Dalton : public Game
{
	public:
		virtual void init();
		virtual void update();
		virtual void draw();

		static Dalton dalton;

	private:
		Dalton() {}

		// sprite's topleft position
		int positions[4];
};

#endif
