#include "Wonder.h"
#include <iostream>

Wonder::Wonder(const std::string name, Cost cost, Effect effect)
	: name(std::move(name)), constructionCost(std::move(cost)), effect(std::move(effect)) {}

bool Wonder::canBuild(Player& player) const {
	// Verificam daca jucatorul are resursele necesare pentru a construi minunea
	// momentam lasam pe true
	return true; 
}

bool Wonder::isBuilt(Player& player, Game& game) {
	if (built) {
		std::cout << "Wonder " << name << " is already built!" << std::endl;
		return true;
	}

	if(canBuild(player)) {
		std::cout << "Can not afford" << name << "!" << std::endl;
		return false;
}

	// ne prefacem aici ca jucatorul plateste 
	std::cout << "Paying cost for " << name << "is ...." << std::endl;
	built = true;

	if (effect) effect(player, game);

	std::cout << "Built wonder: " << name << "\n";
	return true;




