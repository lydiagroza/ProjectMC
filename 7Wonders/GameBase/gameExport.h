
#pragma once

#ifdef DUEL_LOGIC_EXPORTS
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif