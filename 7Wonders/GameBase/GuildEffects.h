#pragma once
#include <memory>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include "CardBase.h"

class Player;

namespace GuildEndGameEffects {
    // Calculăm VP pentru fiecare tip de ghildă conform regulamentului 7 Wonders Duel
    int calculateBuildersGuild(const Player& p1, const Player& p2);
    int calculateMoneylendersGuild(const Player& p1, const Player& p2);
    int calculateScientistsGuild(const Player& p1, const Player& p2);
    int calculateTradersGuild(const Player& p1, const Player& p2);
    int calculateMagistratesGuild(const Player& p1, const Player& p2);
    int calculateTacticiansGuild(const Player& p1, const Player& p2);
    int calculateShipownersGuild(const Player& p1, const Player& p2);

    // Funcție utilitară pentru a procesa toate ghildele unui jucător la final
    int calculateTotalGuildVP(const Player& owner, const Player& opponent);
}
