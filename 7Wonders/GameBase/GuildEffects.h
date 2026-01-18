#pragma once
#include <memory>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include "CardBase.h"

#include "gameExport.h"

class Player;

namespace GuildEndGameEffects {
    // Calculate VP for each guild type according to 7 Wonders Duel rules
    GAME_API int calculateBuildersGuild(const Player& p1, const Player& p2);
    GAME_API int calculateMoneylendersGuild(const Player& p1, const Player& p2);
    GAME_API int calculateScientistsGuild(const Player& p1, const Player& p2);
    GAME_API int calculateTradersGuild(const Player& p1, const Player& p2);
    GAME_API int calculateMagistratesGuild(const Player& p1, const Player& p2);
    GAME_API int calculateTacticiansGuild(const Player& p1, const Player& p2);
    GAME_API int calculateShipownersGuild(const Player& p1, const Player& p2);

    // Utility function to process all guilds of a player at the end
    GAME_API int calculateTotalGuildVP(const Player& owner, const Player& opponent);
}
