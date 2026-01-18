#pragma once
#include "CardBase.h"
#include "Wonder.h"
#include "Board.h"
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <functional>

#include "gameExport.h"

class GAME_API UniversalCardLoader {
public:
    // Public methods to be called in GameSetup
    static std::vector<std::shared_ptr<CardBase>> loadAgeCards(const std::string& filename);
    static std::vector<std::shared_ptr<CardBase>> loadGuilds(const std::string& filename);
    static std::vector<std::shared_ptr<Wonder>> loadWonders(const std::string& filename);

private:
    static std::string trim(std::string s);
    static std::map<Resource, uint8_t> parseCost(const std::string& s);
    static Color parseColor(const std::string& s);
    static std::optional<Symbol> parseSymbol(const std::string& s);
    static std::string translateEffect(const std::string& raw);

    // Function that "translates" effect strings into code logic
    static std::vector<std::function<void(Player&, Board&, Player&)>> parseEffects(const std::string& s);
};