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
    std::vector<std::shared_ptr<ProgressToken>> m_removedProgressTokens;
	std::vector<std::shared_ptr<CardBase>> m_discardPile;
	MilitaryTrack m_militaryTrack;

public:
	Board() = default;

	// Funcții piramidă de cărți
	void setupCards(int era, std::vector<std::shared_ptr<CardBase>>& deck);
	CardNode* getNodeAt(int row, int index);
	void updateVisibility();
	bool isPyramidEmpty() const;
	void printCardsTree(std::ostream& os) const;
	void printChildrenList() const;
	const CardsPyramid& getPyramid() const;

	// Funcții pistă militară
	 MilitaryTrack& getMilitaryTrack();
	const  MilitaryTrack& getMilitaryTrack() const;


	// Funcții jetoane de progres
	void setAvailableProgressTokens(const std::vector<std::shared_ptr<ProgressToken>>& tokens);
	std::vector<std::shared_ptr<ProgressToken>> getAvailableProgressTokens() const;
	void removeAvailableProgressToken(std::shared_ptr<ProgressToken> token);
	void printTokens(std::ostream& fout = std::cout) const;
	
    // Jetoane eliminate/puse deoparte (pentru Marea Bibliotecă)
    void setRemovedProgressTokens(const std::vector<std::shared_ptr<ProgressToken>>& tokens);
    const std::vector<std::shared_ptr<ProgressToken>>& getRemovedProgressTokens() const;
    void removeRemovedProgressToken(std::shared_ptr<ProgressToken> token);

	void printMilitaryTrack(std::ostream& fout = std::cout) const;

	// Funcții teanc de decartare
	void addCardToDiscardPile(std::shared_ptr<CardBase> card);
	const std::vector<std::shared_ptr<CardBase>>& getDiscardPile() const;
	void removeCardFromDiscardPile(std::shared_ptr<CardBase> card);
	void printDiscardPile(std::ostream& fout) const;
	void removeFromDiscardPile(const std::shared_ptr<CardBase>& card);

	friend std::ostream& operator<<(std::ostream& os, const Board& board);
    friend class SaveManager;
};