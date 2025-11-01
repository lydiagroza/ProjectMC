#pragma once
#include <vector>
#include "CardNode.h"
#include <iostream>


class Board
{
private: 
	int Age;
	std::vector<std::vector<CardNode*>> activeCards;
	std::vector<std::vector<int>> eraLayouts = {
	   {2,3,4,5,6},  // Epoca I
	   {6,5,4,3,2},  // Epoca II
	   {2,3,4,2,4,3,2}   // Epoca III
	};


public:
	Board()=default;
	//~Board();

	void setupCards(int era, std::vector<CardNode*>& eraCards);
	//std::vector<CardNode*>& getActiveCards();

	void printBoard() const;

};

