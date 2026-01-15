#include "Game.h"
#include "MilitaryTrack.h"
#include <iostream>
#include <optional> 
#include <vector>
#include <memory>
#include <algorithm>
#include "Wonder.h"


Game* Game::currentGame = nullptr;

Game::Game()
    : m_board(),         
    m_setup(m_board),   
    m_player1("Player 1"),
    m_player2("Player 2"),
    m_currentAge(1),
    m_gameOver(false)
{
    currentGame = this;
    m_currentPlayer = &m_player1;
    m_opponent = &m_player2;
}
void Game::printPlayerInfo(const Player& player, std::ostream& os) const
{
    os << player.getName() << "\n";
}

void Game::switchTurn()
{
    // Schimbăm pointerii între ei
    Player* temp = m_currentPlayer;
    m_currentPlayer = m_opponent;
    m_opponent = temp;
}

bool Game::handleWonderConstruction(std::shared_ptr<CardBase> cardUsed) {

    std::vector<Wonder*> availableWonders;

    for (const auto& wonderPtr : m_currentPlayer->getWonders()) {
        if (wonderPtr && !wonderPtr->getIsBuilt()) {
            availableWonders.push_back(wonderPtr.get());
        }
    }

    if (availableWonders.empty()) {
        std::cout << "Eroare: Nu ai Minuni neconstruite disponibile pentru a construi.\n";
        return false;
    }

    //ii dam minunile disp
    std::cout << "\n--- Minuni Neconstruite Disponibile ---\n";
    for (size_t i = 0; i < availableWonders.size(); ++i)
        std::cout << "[" << i + 1 << "] "<< availableWonders[i]->getName()<< "\n";
   

	//alegem minunea
    std::cout << "Alege numarul Minunii pe care vrei sa o construiesti: ";
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Alegere invalida. Te rog introdu un numar.\n";
        return false;
    }

    size_t index = choice - 1;

    if (index >= availableWonders.size()) {
        std::cout << "Alegere invalida. Incearca din nou.\n";
        return false;
    }

    Wonder* selectedWonder = availableWonders[index];

    m_currentPlayer->constructWonder(
        cardUsed,
        *selectedWonder,
        *m_opponent,
        m_board
    );

    if (selectedWonder->getIsBuilt()) {
        std::cout << m_currentPlayer->getName() << " a construit Minunea: " << selectedWonder->getName() << "\n";
        m_numberOfWondersBuilt++;
        if (m_numberOfWondersBuilt == 7)
            handle7WondersRule();
        return true;
    }
    return false;
}

void Game::handle7WondersRule()
{
	Wonder* lastRemainingWonder = nullptr;
    auto findUnbuiltAvailableWonder = [](Player& p) -> Wonder* {
        for (const auto& wonderPtr : p.getWonders()) {
            // Access Wonder methods via the pointer
            if (wonderPtr && !wonderPtr->getIsBuilt() && wonderPtr->getIsAvailable()) {
                return wonderPtr.get();
            }
        }
        return nullptr;
        };
    lastRemainingWonder = findUnbuiltAvailableWonder(m_player1);

    if (lastRemainingWonder == nullptr) {
        lastRemainingWonder = findUnbuiltAvailableWonder(m_player2);
    }

    if (lastRemainingWonder != nullptr) {
        std::cout<< lastRemainingWonder->getName() << "' este scoasa din joc (returnata la cutie).\n";
        // Marchează Minunea ca indisponibilă, astfel încât să nu mai apară în meniu.
        lastRemainingWonder->returnToBox();
    }
}

    
int Game::calculatePlayerVP(const Player& player) const {
    int totalVP = 0;
    int playerId = (player.getName() == m_player1.getName()) ? 1 : 2;
    
    // 1. Coins
    totalVP += player.getCoins() / 3;
    
    // 2. Military Track
    totalVP += m_board.getMilitaryTrack().getVictoryPointsForPlayer(playerId);
    
    // 3. Military Tokens
    totalVP += player.getVPFromMilitaryTokens();

    // 4. Blue Cards (Civilian Buildings)
    totalVP += player.getVPFromBlueCards();

    // 5. Green Cards (Scientific) - handled via Progress Tokens usually, but cards themselves have VPs sometimes?
    // In Duel, Green cards usually have symbols (6 pts for pairs) but the cards themselves generally provide points on the card face.
    // However, the `CardBase` parsing for generic VPs isn't explicitly summed here. 
    // Usually, VPs are stored in `m_pointsScore` map if they are flat VPs.
    // Let's assume standard VPs are handled in `player.getPoints()`, but `getVPFromBlueCards` does a specific lookup.
    // Let's add general flat VPs if they exist in `m_pointsScore`.
    if (player.getPoints().count(Points::Victory)) {
        totalVP += player.getPoints().at(Points::Victory);
    }

    // 6. Wonders
    for (const auto& wonder : player.getWonders()) {
        if (wonder->getIsBuilt()) {
            // Wonders usually have flat VP effects added to m_pointsScore or need specific handling
            // If WonderLoader adds VPs to m_pointsScore, it's covered above.
            // If not, we might need to check wonders. 
            // For now, let's assume Wonder VPs are added to Player's points map upon construction.
        }
    }

    // 7. Guilds
    const Player& opponent = (player.getName() == m_player1.getName()) ? m_player2 : m_player1;
    totalVP += player.getVPFromGuilds(opponent);

    return totalVP;
}

std::optional<Player> Game::determinateWinner()
{
    int vp1 = calculatePlayerVP(m_player1);
    int vp2 = calculatePlayerVP(m_player2);

	std::cout << "\n--- Rezultatele Finale ---\n";
    std::cout << m_player1.getName() << " Total VP: " << vp1 << "\n";
    std::cout << m_player2.getName() << " Total VP: " << vp2 << "\n";
    if (vp1 > vp2) {
        std::cout << "\n Câștigătorul este: " << m_player1.getName() << "\n";
        return m_player1;
    }
    else if (vp2 > vp1) {
        std::cout << "\n Câștigătorul este: " << m_player2.getName() << "\n";
        return m_player2;
    }
    else {

        int blueVP1 = m_player1.getVPFromBlueCards();
        int blueVP2 = m_player2.getVPFromBlueCards();

        if (blueVP1 > blueVP2) {
            std::cout << "Jocul s-a terminat la egalitate, dar " << m_player1.getName()
                << " castiga la tie-breaker (Clădiri Civile - Albastre).\n";
            return m_player1;
        }
        else if (blueVP2 > blueVP1) {
            std::cout << "Jocul s-a terminat la egalitate, dar " << m_player2.getName()
                << " castiga la tie-breaker (Clădiri Civile - Albastre).\n";
            return m_player2;
        }
        else {
            std::cout << "Jocul s-a terminat la egalitate perfectă. Este remiză.\n";
            return std::nullopt; // Return an empty optional for a draw
        }
    }

}

void Game::handlePlayerAction()
{
    std::cout << "\n>>> ESTE RANDUL LUI: " << m_currentPlayer->getName() << "\n";
    std::cout << "Resurse (Bani): " << (int)m_currentPlayer->getCoins() << "\n";
    std::cout << "Introdu ID-ul cartii de jos pe care vrei sa o iei: ";

    int cardId;
    // Citire sigură (să nu crape dacă bagi litere)
    while (!(std::cin >> cardId)) {
        if (std::cin.eof()) {
            std::cerr << "Input stream ended. Exiting.\n";
            exit(0);
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Te rog introdu un numar valid: ";
    }

    CardNode* selectedNode = nullptr;
    const auto& rows = m_board.getPyramid().getRows();

    for (const auto& row : rows) {
        for (const auto& node : row) {
            // Verificăm ID-ul
            if (node && node->getCard() && node->getCard()->getId() == cardId) {
                // Hack mic: const_cast pentru că selectedNode va fi modificat
                // (Ori faci o funcție în Board care returnează Node* ne-const)
                selectedNode = const_cast<CardNode*>(node.get());
            }
        }
    }

    //Validări
    if (!selectedNode) {
        std::cout << "Eroare: Nu exista carte cu ID-ul " << cardId << "! Incearca iar.\n";
        handlePlayerAction(); // Recursivitate simplă pentru retry
        return;
    }
    if (selectedNode->isPlayed()) {
        std::cout << "Eroare: Cartea a fost deja luata! Incearca iar.\n";
        handlePlayerAction();
        return;
    }
    if (!selectedNode->isPlayable()) {
        std::cout << "Eroare: Cartea este blocata de altele! Incearca iar.\n";
        handlePlayerAction();
        return;
    }

    // 3. Alegerea Acțiunii
    std::cout << "Ai ales: " << selectedNode->getCard()->getName() << "\n";
    std::cout << "Ce faci? [1]=Cumpara, [2]=Vinde, [3]=Construieste minune: ";
    int action;
    std::cin >> action;

    bool success = false;

    if (action == 1) {
        success = m_currentPlayer->buyCard(selectedNode->getCard(), *m_opponent, m_board);
    }
    else if (action == 2) {
        m_currentPlayer->discardCard(*selectedNode->getCard());
        std::cout << "Carte vanduta pentru bani.\n";
        success = true;
    }
    else if (action == 3)
    {
        success = handleWonderConstruction(selectedNode->getCard());
        if (success)
            selectedNode->updatePlayedStatus(true);
    }
    else {
        std::cout << "Actiune invalida.\n";
        handlePlayerAction();
        return;
    }
    if (success) {
        selectedNode->updatePlayedStatus(true);

        m_board.updateVisibility();

        if (!m_currentPlayer->hasExtraTurn()) {
            switchTurn();
        }
        else {
            std::cout << "Ai primit o tura extra! Mai joci o data.\n";
        }
    }
    else {
        std::cout << "Actiunea a esuat. Incearca altceva.\n";
        handlePlayerAction();
    }
}

void Game::printGameState() const {
    std::cout << "\n============================================\n";
    std::cout << "               STAREA JOCULUI (Age " << m_currentAge << ")      \n";
    std::cout << "============================================\n";

    std::cout << "[P1] " << m_player1.getName()
        << " | Bani: " << (int)m_player1.getCoins()
        << " | Minuni: " << m_player1.getWonders().size() << "\n";

    std::cout << "[P2] " << m_player2.getName()
        << " | Bani: " << (int)m_player2.getCoins()
        << " | Minuni: " << m_player2.getWonders().size() << "\n";

    std::cout << "\n--- Pista Militara ---\n";
    m_board.printMilitaryTrack();

    m_board.printCardsTree(std::cout);
    std::cout << "============================================\n";
}

void Game::draftWondersPhase() {
    // Doar pregătim seturile de minuni. Logica de alegere va fi în UI.
    m_draftPhase = 1;
    m_draftSet1 = m_setup.drawWonders(4);
    m_draftSet2 = m_setup.drawWonders(4);

    // Setăm jucătorul care începe draftul (Player 1)
    m_currentPlayer = &m_player1;
    m_opponent = &m_player2;
}

void Game::startNextAge()
{
    m_currentAge++;
    if (m_currentAge > 3) {
        m_gameOver = true;
        return;
    }
    m_setup.startAge(m_currentAge);
}

bool Game::isEndOfAge()
{
    return m_setup.getBoard().isPyramidEmpty();
}

bool Game::isGameOver() const
{
    return m_gameOver;
}


void Game::initialize()
{
    m_currentAge = 1;
    m_gameOver = false;
	draftWondersPhase();
    m_setup.startAge(m_currentAge);
    m_player1.addResource(Coin,7);
    m_player2.addResource(Coin, 7);
    m_board.updateVisibility();
    std::cout << "[Game Logic] Initialization Complete. Age 1 Started. Players have 7 coins.\n";
}

bool Game::checkForInstantWin() {

    int militaryPosition = m_board.getMilitaryTrack().getPawnPosition();

    // Player 1 atinge +9 (sau mai mult)
    if (militaryPosition >= 9) {
		std::cout << "Victorie militara pt " << m_player1.getName() << "\n";
        m_gameOver = true;
        return true;
    }

    // Player 2 atinge -9 (sau mai puțin)
    if (militaryPosition <=- 9) {
        std::cout << "Victorie militara pt " << m_player2.getName() << "\n";
        m_gameOver = true;
        return true;
    }

    //verif suprematie stiintifica

	if (m_player1.getNrOfScientificSymbols() >= 6) //trebuie implementata fct in player
	{
		std::cout << "victorie stiintifica pt " << m_player1.getName() << "\n";
        m_gameOver = true;
        return true;
    }

    if (m_player2.getNrOfScientificSymbols() >= 6) {
		std::cout << "victorie stiintifica pt " << m_player2.getName() << "\n";
        return true;
    }

    return false;
}

// --- Implementarea noilor funcții ---

const std::vector<std::shared_ptr<Wonder>>& Game::getCurrentDraftSet() const {
    if (m_draftPhase == 1) {
        return m_draftSet1;
    }
    return m_draftSet2;
}

bool Game::draftWonder(int wonderId)
{
    auto& currentDraftSet = (m_draftPhase == 1) ? m_draftSet1 : m_draftSet2;

    auto it = std::find_if(currentDraftSet.begin(), currentDraftSet.end(),
        [wonderId](const auto& wonderPtr) {
            return wonderPtr->getId() == wonderId;
        });

    if (it == currentDraftSet.end()) {
        return false; // Wonder not found in the current draft set
    }

    m_currentPlayer->addWonders(*it);
    currentDraftSet.erase(it); // Remove from available wonders

    bool shouldSwitch = (currentDraftSet.size() == 3 || currentDraftSet.size() == 1);

    if (shouldSwitch) {
        switchTurn();
    }

    // If the current draft set is empty, move to the next phase or end the draft
    if (currentDraftSet.empty()) {
        m_draftPhase++; // Move to phase 2 or finish drafting
        if (m_draftPhase == 2) {
            // After the first draft is done, the player who went second now starts.
            switchTurn();
        }
    }

    return true;
}

Player* Game::getCurrentPlayer() {
    return m_currentPlayer;
}

Player* Game::getOpponent() {
    return m_opponent;
}

int Game::getDraftPhase() const {
    return m_draftPhase;
}

void Game::handleBuildFromDiscard()
{
    // TODO: Implement the logic for building from the discard pile.
}
