#pragma once
#include <funstional>

struct Player;
struct Game;

using Effect = std::function<void(Player& player, Game& game)>;