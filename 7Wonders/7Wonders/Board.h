#pragma once
#include <vector>
#include "CardNode.h"
#include "ProgressTokens.h"
#include "MilitaryTrack.h"
#include "CardsPyramid.h"
#include <iostream>
#include <memory>


class Board
{
private:
	int Age;
	CardsPyramid m_pyramid;
	std::vector<std::shared_ptr<ProgressToken>> m_availableProgressTokens;
	std::vector<CardBase*> m_discardPile;
	MilitaryTrack m_militaryTrack;

public:
	Board() = default;

	//functii piramida de carti
	void setupCards(int era, std::vector<CardBase*>& deck);
	CardNode* getNodeAt(int row, int index);
	void updateVisibility();
	bool isPyramidEmpty() const;
	void printCardsTree(std::ostream& os) const;
	void printChildrenList() const;

	//functii tokeni verzi
	void setAvailableProgressTokens(const std::vector<std::shared_ptr<ProgressToken>>& tokens);
	std::vector<std::shared_ptr<ProgressToken>> getAvailableProgressTokens() const;
	void printTokens(std::ostream& fout = std::cout) const;
	void printMilitaryTrack(std::ostream& fout = std::cout) const;

	//functii discard pile
	void addCardToDiscardPile(CardBase* card);
	const std::vector<CardBase*>& getDiscardPile() const;
	void printDiscardPile(std::ostream& fout) const;

	friend std::ostream& operator<<(std::ostream& os, const Board& board);
};