#include "Board.h"
#include "CardBase.h"
#include <iomanip>   // Asigură-te că ai acest include
#include <iostream>
#include <algorithm> // Pentru std::max


// piramida de carti

void Board::setupCards(int era, std::vector<CardBase*>& deck) {
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
    // AICI E SCHIMBAREA: Luam randurile din piramida!
    const auto& rows = m_pyramid.getRows();

    if (rows.empty()) {
        os << "Piramida este goala.\n";
        return;
    }

    // Calcul latime maxima
    size_t max_card_width = 0;
    for (const auto& row : rows) {
        for (const auto& nodePtr : row) {
            const auto* node = nodePtr.get(); // .get() pentru unique_ptr

            // Calculam latimea textului: Nume + Status
            // ATENTIE: Folosim isPlayed() pt a vedea daca mai e pe masa
            if (!node->isPlayed()) {
                std::string status = (node->getFace() == Face::Up ? "[OK]" : "[D]");
                // Nota: Daca e FaceUp, e posibil sa fie blocata sau nu. 
                // Poti folosi isPlayable() daca vrei detalii.

                size_t current_width = node->getCard()->get_name().length() + status.length() + 3;
                if (current_width > max_card_width) max_card_width = current_width;
            }
        }
    }
    if (max_card_width < 10) max_card_width = 10; // Minim rezonabil

    // Calculam latimea totala
    size_t max_row_len = 0;
    for (const auto& row : rows) if (row.size() > max_row_len) max_row_len = row.size();

    const size_t total_width = max_row_len * (max_card_width + 4);

    // Afisare
    os << "\n===== STRUCTURA PIRAMIDEI =====\n\n";

    for (size_t r = 0; r < rows.size(); ++r) {
        const auto& row = rows[r];

        // Spatiere pentru centrare
        size_t current_row_width = row.size() * (max_card_width + 4);
        size_t padding = (total_width > current_row_width) ? (total_width - current_row_width) / 2 : 0;

        os << "[R" << r << "] " << std::string(padding, ' ');

        for (size_t i = 0; i < row.size(); ++i) {
            const auto* node = row[i].get();

            if (node->isPlayed()) {
                // Loc gol
                os << "[" << std::string(max_card_width, '-') << "] ";
            }
            else {
                // Carte existenta
                if (node->getFace() == Face::Down) {
                    os << "(" << i << ")[????] ";
                }
                else {
                    // Verificam daca e jucabila (neblocata)
                    // Atentie: Trebuie sa ai metoda publica isPlayable() in CardNode
                    // Daca nu ai, folosim doar FaceUp.
                    std::string status = "[OK]";

                    std::string name = node->getCard()->get_name();
                    if (name.length() > max_card_width - 4) name = name.substr(0, max_card_width - 4);

                    os << "(" << i << ")[" << name << status << "] ";
                }
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