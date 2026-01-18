#pragma once
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "gameExport.h"

class Player;
class ProgressToken;

class GAME_API ProgressTokenLoader {
public:
    static std::vector<std::function<void(Player&, Player&)>> parseEffects(const std::string& s);
    static std::vector<std::shared_ptr<ProgressToken>> loadProgressTokens(const std::string& filename);
    static std::string trim(std::string s);
};