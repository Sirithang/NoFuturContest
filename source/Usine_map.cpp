#include "Usine_map.hpp"
#include <stdlib.h>
#include <time.h>

using namespace usine;

//size of a case of the usine map
#define USINE_CASE_T 16

void usine::init(UsineMap& obj)
{
	for(int i = 0; i < 12*16; ++i)
		obj.map[i] = 0;

	//create the procedural usine
	// place 5 machine anywhere
	for(int i = 0; i < 5; ++i)
	{
		u8 y = rand()%12;
		u8 x = rand()%16;

		obj.map[x + y*16] = rand()%5+1;
	}
}

//===================================

Vec2i usine::getCase(UsineMap& obj, touchPosition& position)
{
	Vec2i result;

	result.x = position.px / USINE_CASE_T;
	result.y = position.py / USINE_CASE_T;

	return result;
}
