#include "ProgressTokens.h"
#include <sstream>
#include <unordered_map>
#include <iostream>
#include "CardBase.h"


class Player;

const std::string& ProgressToken:: getName() const { return m_name; }

std::ostream& operator<<(std::ostream& os, const ProgressToken& token)
{
    os << "Progress Token ID: " << token.getId() << ", Name: " << token.getName();
	return os;
}
const std::vector<std::function<void(Player&, Player&)>>& ProgressToken :: getEffects() const { return effects; }

void ProgressToken:: addEffect(std::function<void(Player&, Player&)> e) {
    effects.push_back(e);
}