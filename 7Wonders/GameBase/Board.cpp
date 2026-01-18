#include "Board.h"
#include "CardBase.h"
#include <iomanip>   // Asigură-te că ai acest include
#include <iostream>
#include <algorithm> // Pentru std::max
#include <ranges>


// Pyramid functions

void Board::setupCards(int era, std::vector<std::shared_ptr<CardBase>>& deck) {
    m_pyramid.build(era, deck);
    m_pyramid.updateVisibility();
}

CardNode* Board::getNodeAt(int row, int index) {
    return m_pyramid.getNodeAt(row, index);
}

void Board::updateVisibility() {
    m_pyramid.updateVisibility();
}

bool Board::isPyramidEmpty() const {
    return m_pyramid.isEmpty();
}

void Board::setAvailableProgressTokens(const std::vector<std::shared_ptr<ProgressToken>>& tokens)
{
    m_availableProgressTokens = tokens;
}

std::vector<std::shared_ptr<ProgressToken>> Board::getAvailableProgressTokens() const
{
    return m_availableProgressTokens;
}

void Board::removeAvailableProgressToken(std::shared_ptr<ProgressToken> token)
{
	for (auto it = m_availableProgressTokens.begin(); it != m_availableProgressTokens.end(); ++it)
	{
		if ((*it)->getId() == token->getId())
		{
			m_availableProgressTokens.erase(it);
			return;
		}
	}
}

// Removed Progress Tokens
void Board::setRemovedProgressTokens(const std::vector<std::shared_ptr<ProgressToken>>& tokens)
{
    m_removedProgressTokens = tokens; // Assuming m_removedProgressTokens is defined in header now
}

const std::vector<std::shared_ptr<ProgressToken>>& Board::getRemovedProgressTokens() const
{
    return m_removedProgressTokens;
}

void Board::removeRemovedProgressToken(std::shared_ptr<ProgressToken> token)
{
    for (auto it = m_removedProgressTokens.begin(); it != m_removedProgressTokens.end(); ++it)
    {
        if ((*it)->getId() == token->getId())
        {
            m_removedProgressTokens.erase(it);
            return;
        }
    }
}


void Board::printTokens(std::ostream& os) const
{
    if (m_availableProgressTokens.empty()) {
        os << "No available progress tokens." << std::endl;
        return;
    }

    for (const auto& tokenPtr : m_availableProgressTokens) {
        if (tokenPtr) {
            os << *tokenPtr << "\n"; // Use the stream operator
        }
    }
    os << std::endl;
}

void Board::printMilitaryTrack([[maybe_unused]] std::ostream& fout) const
{

	int currentPos = m_militaryTrack.getPawnPosition();
    std::cout << "\n[Military]: P2 ";

    for (int i = -9; i <= 9; ++i)
    {
    
        if (i == currentPos) {
            std::cout << "(X)";
        }
        else if (i == -6) {
            if (m_militaryTrack.isTokenVisible(0)) std::cout << "[5]";
            else std::cout << " . ";
        }
        else if (i == -3) {
            if (m_militaryTrack.isTokenVisible(1)) std::cout << "[2]";
            else std::cout << " . ";
        }
        else if (i == 3) {
            if (m_militaryTrack.isTokenVisible(2)) std::cout << "[2]";
            else std::cout << " . ";
        }
        else if (i == 6) {
            if (m_militaryTrack.isTokenVisible(3)) std::cout << "[5]";
            else std::cout << " . ";
        }
        else if (i == 0) {
            std::cout << " | ";
        }
        else {
            std::cout << " . ";
        }
    }

    std::cout << " P1\n";

}

void Board::addCardToDiscardPile(std::shared_ptr<CardBase> card)
{
    if (card) {
        m_discardPile.push_back(card);
    }
}

const std::vector<std::shared_ptr<CardBase>>& Board::getDiscardPile() const
{
    return m_discardPile;
}

void Board::printDiscardPile(std::ostream& fout) const
{
	fout << "\n=== Discard Pile ===\n";
    if(m_discardPile.empty()) {
        fout << "Nu exista carti in discard pile.\n";
        return;
    }
    std::cout << "[ ";
    for (const auto& card : m_discardPile)
        std::cout << card->getName() << ", ";

    std::cout << "]\n";
}

void Board::removeFromDiscardPile(const std::shared_ptr<CardBase>& card)
{
    auto it = std::ranges::find_if(m_discardPile,
        [&card](const std::shared_ptr<CardBase>& c) {
            return c->getId() == card->getId();
        });
    if (it != m_discardPile.end()) {
        m_discardPile.erase(it);
	}
}

void Board::removeCardFromDiscardPile(std::shared_ptr<CardBase> card)
{
    for (auto it = m_discardPile.begin(); it != m_discardPile.end(); ++it)
    {
        if ((*it)->getId() == card->getId())
        {
            m_discardPile.erase(it);
            return;
        }
    }
}

void Board::printCardsTree(std::ostream& os) const
{
    const auto& rows = m_pyramid.getRows();

    if (rows.empty()) {
        os << "Piramida este goala.\n";
        return;
    }

    size_t max_text_width = 0;
    for (const auto& row : rows) {
        for (const auto& nodePtr : row) {
            const auto* node = nodePtr.get();
            if (!node->isPlayed()) {
                size_t len = node->getCard()->getName().length() + 6;
                if (len > max_text_width) max_text_width = len;
            }
        }
    }
    if (max_text_width < 15) max_text_width = 15;

    size_t max_cards_in_row = 0;
    for (const auto& row : rows) {
        if (row.size() > max_cards_in_row) max_cards_in_row = row.size();
    }

    os << "\n===== STRUCTURA PIRAMIDEI =====\n\n";

    for (size_t r = 0; r < rows.size(); ++r) {
        const auto& row = rows[r];

        auto block_size = max_text_width + 4;

        auto missing_blocks = (double)(max_cards_in_row - row.size()) / 2.0;

        auto padding = (size_t)(missing_blocks * block_size);

        os << std::string(padding, ' ');

        for (size_t i = 0; i < row.size(); ++i) {
            const auto* node = row[i].get();

            if (node->isPlayed()) {
                os << "[" << std::string(max_text_width, '-') << "]  ";
            }
            else {
                std::string content;

                if (node->getFace() == Face::Down) {
                    content = "[????]";
                }
                else {
                    std::string status = (node->isPlayable() ? "[OK]" : "[BLK]"); 
                    std::string name = node->getCard()->getName();
                    std::string id_prefix = std::to_string(node->getCard()->getId());
                    if (name.length() > max_text_width - 5) name = name.substr(0, max_text_width - 5);
                    content = id_prefix + name + " " + status;
                }

                os << std::left << std::setw(max_text_width) << content;
            }
        }
        os << "\n\n"; 
    }
    os << "===============================\n";
}

void Board::printChildrenList() const {
    const auto& rows = m_pyramid.getRows();
    std::cout << "\n--- DEBUG: Relatii Blocare ---\n";

    for (const auto& row : rows) {
        for (const auto& nodePtr : row) {
            const auto* node = nodePtr.get();
            if (node->isPlayed()) continue;

            std::cout << "-> " << node->getCard()->getName();

            const auto& children = node->getChildren();
            if (children.empty()) std::cout << " [LIBER]\n";
            else {
                std::cout << " [BLOCAT DE]: ";
                for (const auto* child : children) {
                    if (!child->isPlayed()) // Aratam doar copiii care inca sunt pe masa
                        std::cout << child->getCard()->getName() << ", ";
                }
                std::cout << "\n";
            }
        }
    }
}

const CardsPyramid& Board::getPyramid() const
{
	return m_pyramid;
}

MilitaryTrack& Board::getMilitaryTrack() {
	return m_militaryTrack;
}

const MilitaryTrack& Board::getMilitaryTrack() const
{
    return m_militaryTrack;
}

std::ostream& operator<<(std::ostream& os, const Board& board)
{
    os << "========== GAME BOARD STATE ==========\n\n";

    os << "--- Card Pyramid ---\n";
    board.printCardsTree(os);
    os << "\n";

    os << "--- Military Track ---\n";
    board.printMilitaryTrack(os); 
    os << "\n";

    os << "--- Available Progress Tokens ---\n";
    board.printTokens(os); 
    os << "\n";

    os << "======================================\n";

    return os; 
}