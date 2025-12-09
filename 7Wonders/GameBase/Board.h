#pragma once
#include <vector>
#include "CardNode.h"
#include "ProgressTokens.h"
#include "MilitaryTrack.h"
#include "CardsPyramid.h"
#include <iostream>
#include <memory>
#include "gameExport.h"


class GAME_API Board
{
private:
	int Age;
	CardsPyramid m_pyramid;
	std::vector<std::shared_ptr<ProgressToken>> m_availableProgressTokens;
	std::vector<std::shared_ptr<CardBase>> m_discardPile;
	MilitaryTrack m_militaryTrack;

public:
	Board() = default;

	//functii piramida de carti
	void setupCards(int era, std::vector<std::shared_ptr<CardBase>>& deck);
	CardNode* getNodeAt(int row, int index);
	void updateVisibility();
	bool isPyramidEmpty() const;
	void printCardsTree(std::ostream& os) const;
	void printChildrenList() const;

	//functii tokeni verzi
	void setAvailableProgressTokens(const std::vector<std::shared_ptr<ProgressToken>>& tokens);
	std::vector<std::shared_ptr<ProgressToken>> getAvailableProgressTokens() const;
	void removeAvailableProgressToken(std::shared_ptr<ProgressToken> token);
	void printTokens(std::ostream& fout = std::cout) const;
	void printMilitaryTrack(std::ostream& fout = std::cout) const;

	//functii discard pile
	void addCardToDiscardPile(std::shared_ptr<CardBase> card);
	const std::vector<std::shared_ptr<CardBase>>& getDiscardPile() const;
	void removeCardFromDiscardPile(std::shared_ptr<CardBase> card);
	void printDiscardPile(std::ostream& fout) const;

	friend std::ostream& operator<<(std::ostream& os, const Board& board);
};