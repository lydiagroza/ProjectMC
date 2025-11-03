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

	//legaturile parinti-copii pt era 1
	std::vector<std::vector<std::vector<int>>> eraChildIndexes1 = {
	{ {0,1}, {1,2} },                // Row 0 -> Row 1 (2 cărți)
	{ {0,1}, {1,2}, {2,3} },         // Row 1 -> Row 2 (3 cărți)
	{ {0,1}, {1,2}, {2,3}, {3,4} },  // Row 2 -> Row 3 (4 cărți)
	{ {0,1}, {1,2}, {2,3}, {3,4}, {4,5} } // Row 3 -> Row 4 (5 cărți)
	// Row 4 (6 cărți) nu are copii
	};

	//legaturile parinti-copii pt era 2
	std::vector<std::vector<std::vector<int>>> eraChildIndexes2 = {
	{ {0}, {0,1} , {1,2}, {2,3}, {3,4}, {4}},
	{ {0}, {0,1} , {1,2}, {2,3}, {3}},
	{ {0}, {0,1} , {1,2}, {2}},
	{ {0}, {0,1},{1} }
	};

	//legaturile parinti-copii pt era 3
	std::vector<std::vector<std::vector<int>>> eraChildIndexes3 = {
	{ {0,1}, {1,2} },
	{ {0,1}, {1,2}, {2,3} },
	{ {0},{0},{1},{1}},
	{ {0,1},{2,3} },
	{ {0}, { 0,1 }, {1,2} ,{2}},
	{ {0}, { 0,1 },{1} },
	};

public:
	Board()=default;
	//~Board();

	void setupCards(int era, std::vector<CardNode*>& eraCards);
	//std::vector<CardNode*>& getActiveCards();

	void printBoard() const;
	void printChildrenList() const;
	void linkCards(int);

};

