#ifndef COURONNE_HPP
#define COURONNE_HPP

#include "Game.hpp"
#include "Vector_math.inl"

class Couronne : public Game
{
	public:
		virtual void init();
		virtual void update();
		virtual void draw();

		static Couronne couronne;

	private:
		Couronne() {}

		
		Vec2i couronne_position;
		Vec2i last_pos;

		bool touched;
		bool grabbed;
};

#endif
