#pragma once
#include <vector>
#include <memory>
#include "CardNode.h"
#include "gameExport.h"

class CardBase;

class GAME_API CardsPyramid
{
	std::vector<std::vector<std::unique_ptr<CardNode>>> m_rows;
	void clear();

	std::vector<std::vector<int>> m_ageLayouts = {
	   {2,3,4,5,6},  // Age I
	   {6,5,4,3,2},  // Age II
	   {2,3,4,2,4,3,2}   // Age III
	};

	//parent-child links for age 1
	std::vector<std::vector<std::vector<int>>> m_ageChildIndexes1 = {
	{ {0,1}, {1,2} },                // Row 0 -> Row 1 (2 cards)
	{ {0,1}, {1,2}, {2,3} },         // Row 1 -> Row 2 (3 cards)
	{ {0,1}, {1,2}, {2,3}, {3,4} },  // Row 2 -> Row 3 (4 cards)
	{ {0,1}, {1,2}, {2,3}, {3,4}, {4,5} } // Row 3 -> Row 4 (5 cards)
	// Row 4 (6 cards) has no children
	};

	//parent-child links for age 2
	std::vector<std::vector<std::vector<int>>> m_ageChildIndexes2 = {
	{ {0}, {0,1} , {1,2}, {2,3}, {3,4}, {4}},
	{ {0}, {0,1} , {1,2}, {2,3}, {3}},
	{ {0}, {0,1} , {1,2}, {2}},
	{ {0}, {0,1},{1} }
	};

	//parent-child links for age 3
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

	CardsPyramid(const CardsPyramid&) = delete;
	CardsPyramid& operator=(const CardsPyramid&) = delete;

	CardsPyramid(CardsPyramid&&) = default;
	CardsPyramid& operator=(CardsPyramid&&) = default;

	void build(int age, std::vector<std::shared_ptr<CardBase>>& deck);
	void linkCards(int age);

	CardNode* getNodeAt(size_t row, size_t col) const;
	const std::vector<std::vector<std::unique_ptr<CardNode>>>& getRows() const;

	void updateVisibility();

	bool isEmpty() const;
	
    friend class SaveManager;
};

