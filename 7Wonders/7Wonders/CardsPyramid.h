#pragma once
#include <vector>
#include <memory>
#include "CardNode.h"

class CardBase;

class CardsPyramid
{
	std::vector<std::vector<std::unique_ptr<CardNode>>> m_rows;
	void clear();

	std::vector<std::vector<int>> m_ageLayouts = {
	   {2,3,4,5,6},  // Epoca I
	   {6,5,4,3,2},  // Epoca II
	   {2,3,4,2,4,3,2}   // Epoca III
	};

	//legaturile parinti-copii pt era 1
	std::vector<std::vector<std::vector<int>>> m_ageChildIndexes1 = {
	{ {0,1}, {1,2} },                // Row 0 -> Row 1 (2 cărți)
	{ {0,1}, {1,2}, {2,3} },         // Row 1 -> Row 2 (3 cărți)
	{ {0,1}, {1,2}, {2,3}, {3,4} },  // Row 2 -> Row 3 (4 cărți)
	{ {0,1}, {1,2}, {2,3}, {3,4}, {4,5} } // Row 3 -> Row 4 (5 cărți)
	// Row 4 (6 cărți) nu are copii
	};

	//legaturile parinti-copii pt era 2
	std::vector<std::vector<std::vector<int>>> m_ageChildIndexes2 = {
	{ {0}, {0,1} , {1,2}, {2,3}, {3,4}, {4}},
	{ {0}, {0,1} , {1,2}, {2,3}, {3}},
	{ {0}, {0,1} , {1,2}, {2}},
	{ {0}, {0,1},{1} }
	};

	//legaturile parinti-copii pt era 3
	std::vector<std::vector<std::vector<int>>> m_ageChildIndexes3 = {
	{ {0,1}, {1,2} },
	{ {0,1}, {1,2}, {2,3} },
	{ {0},{0},{1},{1}},
	{ {0,1},{2,3} },
	{ {0}, { 0,1 }, {1,2} ,{2}},
	{ {0}, { 0,1 },{1} },
	};


public:
	CardsPyramid()=default;

	void build(int age, std::vector<CardBase*>& deck);

	void linkCards(int age);

	CardNode* getNodeAt(size_t row, size_t col) const;
	const std::vector<std::vector<std::unique_ptr<CardNode>>>& getRows() const;

	void updateVisibility();

	bool isEmpty() const;
	
};

