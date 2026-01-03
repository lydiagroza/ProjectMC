
#pragma once

#ifdef GAMEBASE_STATIC
#define GAME_API
#elif defined(DUEL_LOGIC_EXPORTS)
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif
