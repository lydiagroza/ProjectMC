#include "Board.h"
#include "CardBase.h"
#include <iomanip>   // Asigură-te că ai acest include
#include <iostream>
#include <algorithm> // Pentru std::max


// piramida de carti

void Board::setupCards(int era, std::vector<std::shared_ptr<CardBase>>& deck) {
    // Delegam toata munca grea clasei specializate
    m_pyramid.build(era, deck);
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


//tokenii aia verzi
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

//tabla militara

void Board::printMilitaryTrack(std::ostream& fout) const
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
            else std::cout << " . "; // Tokenul a fost luat
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
//carti "arse"
void Board::addCardToDiscardPile(CardBase* card)
{
    if (card) {
        m_discardPile.push_back(card);
    }
}

const std::vector<CardBase*>& Board::getDiscardPile() const
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
    for (const auto* card : m_discardPile)
        std::cout << card->get_name() << ", ";

    std::cout << "]\n";
}

//arbore de carti
void Board::printCardsTree(std::ostream& os) const
{
    const auto& rows = m_pyramid.getRows();

    if (rows.empty()) {
        os << "Piramida este goala.\n";
        return;
    }

    // 1. Aflăm cea mai lată denumire de carte (pentru a face blocurile egale)
    size_t max_text_width = 0;
    for (const auto& row : rows) {
        for (const auto& nodePtr : row) {
            const auto* node = nodePtr.get();
            if (!node->isPlayed()) {
                // Lungimea numelui + statusul [OK]/[D]
                size_t len = node->getCard()->get_name().length() + 6;
                if (len > max_text_width) max_text_width = len;
            }
        }
    }
    // Setăm o lățime minimă fixă ca să arate bine
    if (max_text_width < 15) max_text_width = 15;

    // 2. Aflăm care este cel mai lung rând (baza piramidei)
    // În Age 1 e rândul de jos (6 cărți). În Age 3 e mijlocul (4 cărți).
    size_t max_cards_in_row = 0;
    for (const auto& row : rows) {
        if (row.size() > max_cards_in_row) max_cards_in_row = row.size();
    }

    os << "\n===== STRUCTURA PIRAMIDEI =====\n\n";

    // 3. Afișare
    for (size_t r = 0; r < rows.size(); ++r) {
        const auto& row = rows[r];

        // --- CALCUL CENTRARE ---
        // Cât spațiu ocupă o singură carte + spațiul dintre ele
        size_t block_size = max_text_width + 4;

        // Câte "blocuri" lipsesc pe acest rând față de cel mai lung rând?
        double missing_blocks = (double)(max_cards_in_row - row.size()) / 2.0;

        // Transformăm blocurile în spații goale
        size_t padding = (size_t)(missing_blocks * block_size);

        // Afișăm padding-ul din stânga
        os << std::string(padding, ' ');

        // Afișăm cărțile
        for (size_t i = 0; i < row.size(); ++i) {
            const auto* node = row[i].get();

            if (node->isPlayed()) {
                // Loc gol (carte luată) - păstrăm dimensiunea blocului
                os << "[" << std::string(max_text_width, '-') << "]  ";
            }
            else {
                // Carte existentă
                std::string content;

                if (node->getFace() == Face::Down) {
                    content = "[????]";
                }
                else {
                    std::string status = (node->isPlayable() ? "[OK]" : "[BLK]"); // BLK = Blocat
                    // Trunchiem numele dacă e prea lung
                    std::string name = node->getCard()->get_name();
                    if (name.length() > max_text_width - 5) name = name.substr(0, max_text_width - 5);
                    content = name + " " + status;
                }

                // Folosim setw pentru a forța fiecare carte să aibă aceeași lățime vizuală
                os << std::left << std::setw(max_text_width) << content;
            }
        }
        os << "\n\n"; // Rând nou
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

            std::cout << "-> " << node->getCard()->get_name();

            const auto& children = node->getChildren();
            if (children.empty()) std::cout << " [LIBER]\n";
            else {
                std::cout << " [BLOCAT DE]: ";
                for (const auto* child : children) {
                    if (!child->isPlayed()) // Aratam doar copiii care inca sunt pe masa
                        std::cout << child->getCard()->get_name() << ", ";
                }
                std::cout << "\n";
            }
        }
    }
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