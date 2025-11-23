#pragma once
#include <vector>
#include "CardNode.h"
#include "ProgressTokens.h"
#include "MilitaryTrack.h"
#include <iostream>
#include <memory>


class Board
{
private:
	int Age;
	std::vector<std::shared_ptr<ProgressToken>> m_availableProgressTokens;
	std::vector<std::vector<CardNode*>> m_activeCards;
	std::vector<CardBase*> m_discardPile;


	std::vector<std::vector<int>> m_eraLayouts = {
	   {2,3,4,5,6},  // Epoca I
	   {6,5,4,3,2},  // Epoca II
	   {2,3,4,2,4,3,2}   // Epoca III
	};

	//legaturile parinti-copii pt era 1
	std::vector<std::vector<std::vector<int>>> m_eraChildIndexes1 = {
	{ {0,1}, {1,2} },                // Row 0 -> Row 1 (2 cărți)
	{ {0,1}, {1,2}, {2,3} },         // Row 1 -> Row 2 (3 cărți)
	{ {0,1}, {1,2}, {2,3}, {3,4} },  // Row 2 -> Row 3 (4 cărți)
	{ {0,1}, {1,2}, {2,3}, {3,4}, {4,5} } // Row 3 -> Row 4 (5 cărți)
	// Row 4 (6 cărți) nu are copii
	};

	//legaturile parinti-copii pt era 2
	std::vector<std::vector<std::vector<int>>> m_eraChildIndexes2 = {
	{ {0}, {0,1} , {1,2}, {2,3}, {3,4}, {4}},
	{ {0}, {0,1} , {1,2}, {2,3}, {3}},
	{ {0}, {0,1} , {1,2}, {2}},
	{ {0}, {0,1},{1} }
	};

	//legaturile parinti-copii pt era 3
	std::vector<std::vector<std::vector<int>>> m_eraChildIndexes3 = {
	{ {0,1}, {1,2} },
	{ {0,1}, {1,2}, {2,3} },
	{ {0},{0},{1},{1}},
	{ {0,1},{2,3} },
	{ {0}, { 0,1 }, {1,2} ,{2}},
	{ {0}, { 0,1 },{1} },
	};

	MilitaryTrack m_militaryTrack;

public:
	Board() = default;
	void setupCards(int era, std::vector<CardBase*>& deck);

	void printCardsTree() const;
	void printChildrenList() const;
	void linkCards(int);
	bool isPyramidEmpty() const;

	void setAvailableProgressTokens(const std::vector<std::shared_ptr<ProgressToken>>& tokens);
	std::vector<std::shared_ptr<ProgressToken>> getAvailableProgressTokens() const;
	void printTokens(std::ostream& fout = std::cout) const;
	void printMilitaryTrack(std::ostream& fout = std::cout) const;


	//carti "arse"
	void addCardToDiscardPile(CardBase* card);
	const std::vector<CardBase*>& getDiscardPile() const;
	void printDiscardPile(std::ostream& fout) const;


	void updateVisibility();
};