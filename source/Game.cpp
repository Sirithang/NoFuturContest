#include "Game.hpp"
#include "GameHub.hpp"

Game* Game::current;
Game* Game::games[] = {};
const int Game::game_count = sizeof(Game::games)/sizeof(Game*);

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
	return current == &GameHub::hub;
}
