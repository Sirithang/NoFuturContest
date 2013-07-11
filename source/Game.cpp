#include "Game.hpp"
#include "GameHub.hpp"
#include "Capitalist.hpp"

Game* Game::current;
Game* const Game::games[] = { &Capitalist::capitalist };
const int Game::game_count = sizeof(Game::games)/sizeof(Game*);

void Game::init_timer(unsigned char sec, unsigned char frame)
{
	timer_sec = sec;
	timer_frame = frame;
}

void Game::update()
{
	if (timer_frame == 0)
	{
		if (timer_sec == 0)
		{
			game_end(false);
		}
		else
		{
			--timer_sec;
			timer_frame = 59;
		}
	}
	else
	{
		--timer_frame;
	}
}

void Game::start_game(int game)
{
	current = games[game];
	current->init();
}

void Game::game_end(bool success)
{
	GameHub::hub.game_succeed = true;
	GameHub::hub.resume();

	current = &GameHub::hub;
}

bool Game::is_game_playing()
{
	return current != &GameHub::hub;
}
