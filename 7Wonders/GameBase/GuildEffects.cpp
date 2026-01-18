#include "GuildEffects.h"
#include "Player.h"
#include "Wonder.h"
#include "CardBase.h"
#include <iostream>
#include <algorithm>

namespace GuildEndGameEffects {

    static int countCardsByColor(const Player& p, Color c) {
        const auto& inv = p.getInventory();
        auto it = inv.find(c);
        if (it != inv.end()) {
            return (int)it->second.size();
        }
        return 0;
    }

    GAME_API int calculateBuildersGuild(const Player& p1, const Player& p2) {
        auto countWonders = [](const Player& p) {
            int count = 0;
            for (const auto& w : p.getWonders()) {
                if (w && w->getIsBuilt()) count++;
            }
            return count;
        };
        int c1 = countWonders(p1);
        int c2 = countWonders(p2);
        int res = std::max(c1, c2) * 2;
        std::cout << "[Guild Debug] Builders Guild (ID 67): P1 wonders=" << c1 << ", P2 wonders=" << c2 << " -> VP=" << res << std::endl;
        return res;
    }

    GAME_API int calculateMoneylendersGuild(const Player& p1, const Player& p2) {
        int coins1 = p1.getCoins();
        int coins2 = p2.getCoins();
        int res = std::max(coins1, coins2) / 3;
        std::cout << "[Guild Debug] Moneylenders Guild (ID 68): P1 coins=" << coins1 << ", P2 coins=" << coins2 << " -> VP=" << res << std::endl;
        return res;
    }

    GAME_API int calculateScientistsGuild(const Player& p1, const Player& p2) {
        int c1 = countCardsByColor(p1, Color::Green);
        int c2 = countCardsByColor(p2, Color::Green);
        int res = std::max(c1, c2);
        std::cout << "[Guild Debug] Scientists Guild (ID 69): P1 green=" << c1 << ", P2 green=" << c2 << " -> VP=" << res << std::endl;
        return res;
    }

    GAME_API int calculateShipownersGuild(const Player& p1, const Player& p2) {
        auto countBrownGray = [](const Player& p) {
            return countCardsByColor(p, Color::Brown) + countCardsByColor(p, Color::Gray);
        };
        int c1 = countBrownGray(p1);
        int c2 = countBrownGray(p2);
        int res = std::max(c1, c2);
        std::cout << "[Guild Debug] Shipowners Guild (ID 70): P1 brown+gray=" << c1 << ", P2 brown+gray=" << c2 << " -> VP=" << res << std::endl;
        return res;
    }

    GAME_API int calculateTradersGuild(const Player& p1, const Player& p2) {
        int c1 = countCardsByColor(p1, Color::Yellow);
        int c2 = countCardsByColor(p2, Color::Yellow);
        int res = std::max(c1, c2);
        std::cout << "[Guild Debug] Traders Guild (ID 71): P1 yellow=" << c1 << ", P2 yellow=" << c2 << " -> VP=" << res << std::endl;
        return res;
    }

    GAME_API int calculateMagistratesGuild(const Player& p1, const Player& p2) {
        int c1 = countCardsByColor(p1, Color::Blue);
        int c2 = countCardsByColor(p2, Color::Blue);
        int res = std::max(c1, c2);
        std::cout << "[Guild Debug] Magistrates Guild (ID 72): P1 blue=" << c1 << ", P2 blue=" << c2 << " -> VP=" << res << std::endl;
        return res;
    }

    GAME_API int calculateTacticiansGuild(const Player& p1, const Player& p2) {
        int c1 = countCardsByColor(p1, Color::Red);
        int c2 = countCardsByColor(p2, Color::Red);
        int res = std::max(c1, c2);
        std::cout << "[Guild Debug] Tacticians Guild (ID 73): P1 red=" << c1 << ", P2 red=" << c2 << " -> VP=" << res << std::endl;
        return res;
    }

    GAME_API int calculateTotalGuildVP(const Player& owner, const Player& opponent) {
        int total = 0;
        const auto& inventory = owner.getInventory();
        
        auto it = inventory.find(Color::Purple);
        if (it != inventory.end()) {
            for (const auto& card : it->second) {
                if (!card) continue;
                int id = card->getId();
                int guildVP = 0;
                switch (id) {
                    case 67: guildVP = calculateBuildersGuild(owner, opponent); break;
                    case 68: guildVP = calculateMoneylendersGuild(owner, opponent); break;
                    case 69: guildVP = calculateScientistsGuild(owner, opponent); break;
                    case 70: guildVP = calculateShipownersGuild(owner, opponent); break;
                    case 71: guildVP = calculateTradersGuild(owner, opponent); break;
                    case 72: guildVP = calculateMagistratesGuild(owner, opponent); break;
                    case 73: guildVP = calculateTacticiansGuild(owner, opponent); break;
                    default: 
                        std::cout << "[Guild Debug] Unknown Guild ID: " << id << " Name: " << card->getName() << std::endl;
                        break;
                }
                total += guildVP;
            }
        }
        return total;
    }
}