#pragma once
#include <memory>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include "CardBase.h"

class Player;

namespace GuildEndGameEffects {
    // Calculate VP for each guild type according to 7 Wonders Duel rules
    int calculateBuildersGuild(const Player& p1, const Player& p2);
    int calculateMoneylendersGuild(const Player& p1, const Player& p2);
    int calculateScientistsGuild(const Player& p1, const Player& p2);
    int calculateTradersGuild(const Player& p1, const Player& p2);
    int calculateMagistratesGuild(const Player& p1, const Player& p2);
    int calculateTacticiansGuild(const Player& p1, const Player& p2);
    int calculateShipownersGuild(const Player& p1, const Player& p2);

    // Utility function to process all guilds of a player at the end
    int calculateTotalGuildVP(const Player& owner, const Player& opponent);
}
