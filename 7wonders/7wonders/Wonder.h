#pragma once
#include <string>
#include <vector>
#include "Effect.h"
#include "Cost.h"

struct Player; 
struct Game;

class Wonder {
private:
	std::string name;
	Cost constructionCost;
	bool built = false;
	Effect effect;

public:
	Wonder(const std::string name, Cost cost, Effect effect);

	bool canBuild(Player& player) const; // verifica daca player are resurse ca sa contruiasca the wonder

	bool Build(Player& player, Game& game) const;

	const std::string& getName() const { return name; };
	const Cost& getCost() const {return constructionCost;}
	bool canBuild() const {return built;}		

};

