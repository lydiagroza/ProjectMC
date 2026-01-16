#include "Game.h"
#include "MilitaryTrack.h"
#include "AI_Player.h"
#include <iostream>
#include <optional> 
#include <vector>
#include <memory>
#include <algorithm>
#include <limits>
#include "Wonder.h"
#include "UniversalCardLoader.h"


Game* Game::currentGame = nullptr;

Game::Game()
    : m_board(),         
    m_setup(m_board),   
    m_player1(std::make_shared<Player>("Player 1", 1)),
    m_player2(std::make_shared<Player>("Player 2", 2)),
    m_currentAge(1),
    m_gameOver(false)
{
    currentGame = this;
    m_currentPlayer = m_player1.get();
    m_opponent = m_player2.get();
}

void Game::handleProgressTokenChoice()
{
    // Check if there are tokens to choose from
    if (m_board.getAvailableProgressTokens().empty()) {
        return;
    }
    m_isWaitingForProgressTokenChoice = true;
}

void Game::resolveProgressTokenChoice(int tokenId)
{
    if (!m_isWaitingForProgressTokenChoice) return;

    auto available = m_board.getAvailableProgressTokens();
    std::shared_ptr<ProgressToken> chosen = nullptr;
    for (auto t : available) {
        if (t->getId() == tokenId) {
            chosen = t;
            break;
        }
    }

    if (chosen) {
        m_currentPlayer->addProgressToken(chosen);
        for (const auto& effect : chosen->getEffects()) {
            effect(*m_currentPlayer, *m_opponent);
        }
        m_board.removeAvailableProgressToken(chosen);
    }

    m_isWaitingForProgressTokenChoice = false;
}

bool Game::isWaitingForProgressTokenChoice() const
{
    return m_isWaitingForProgressTokenChoice;
}

void Game::handleGreatLibraryChoice()
{
    if (m_board.getRemovedProgressTokens().empty()) {
        std::cout << "[INFO] No set-aside tokens for Great Library.\n";
        return;
    }
    m_isWaitingForGreatLibraryChoice = true;
}

bool Game::isWaitingForGreatLibraryChoice() const
{
    return m_isWaitingForGreatLibraryChoice;
}

void Game::resolveGreatLibraryChoice(int tokenId)
{
    if (!m_isWaitingForGreatLibraryChoice) return;

    auto removed = m_board.getRemovedProgressTokens();
    std::shared_ptr<ProgressToken> chosen = nullptr;
    for (auto t : removed) {
        if (t->getId() == tokenId) {
            chosen = t;
            break;
        }
    }

    if (chosen) {
        m_currentPlayer->addProgressToken(chosen);
        for (const auto& effect : chosen->getEffects()) {
            effect(*m_currentPlayer, *m_opponent);
        }
        m_board.removeRemovedProgressToken(chosen);
    }

    m_isWaitingForGreatLibraryChoice = false;
}

void Game::setPlayerTypes(bool p1IsAI, bool p2IsAI, AI_Difficulty difficulty)
{
    std::string n1 = m_player1->getName();
    std::string n2 = m_player2->getName();

    if (p1IsAI) m_player1 = std::make_shared<AI_Player>(n1, 1, difficulty);
    else m_player1 = std::make_shared<Player>(n1, 1);

    if (p2IsAI) m_player2 = std::make_shared<AI_Player>(n2, 2, difficulty);
    else m_player2 = std::make_shared<Player>(n2, 2);

    // Reset pointers
    m_currentPlayer = m_player1.get();
    m_opponent = m_player2.get();
}

void Game::printPlayerInfo(const Player& player, std::ostream& os) const
{
    os << player.getName() << "\n";
}

void Game::switchTurn()
{
    if (m_currentPlayer == m_player1.get()) {
        m_currentPlayer = m_player2.get();
        m_opponent = m_player1.get();
    } else {
        m_currentPlayer = m_player1.get();
        m_opponent = m_player2.get();
    }
    std::cout << "[Turn] Switched to " << m_currentPlayer->getName() << " (ptr: " << m_currentPlayer << ")\n";
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
    lastRemainingWonder = findUnbuiltAvailableWonder(*m_player1);

    if (lastRemainingWonder == nullptr) {
        lastRemainingWonder = findUnbuiltAvailableWonder(*m_player2);
    }

    if (lastRemainingWonder != nullptr) {
        std::cout<< lastRemainingWonder->getName() << "' este scoasa din joc (returnata la cutie).\n";
        // Marchează Minunea ca indisponibilă, astfel încât să nu mai apară în meniu.
        lastRemainingWonder->returnToBox();
    }
}

    
int Game::calculatePlayerVP(const Player& player) const {
    int totalVP = 0;
    int playerId = (player.getName() == m_player1->getName()) ? 1 : 2;
    
    // 1. Coins
    totalVP += player.getCoins() / 3;
    
    // 2. Military Track
    totalVP += m_board.getMilitaryTrack().getVictoryPointsForPlayer(playerId);
    
    // 3. Military Tokens
    totalVP += player.getVPFromMilitaryTokens();

    // 4. Blue Cards (Civilian Buildings)
    totalVP += player.getVPFromBlueCards();

    // 5. Green Cards (Scientific) - handled via Progress Tokens usually, but cards themselves have VPs sometimes?
    if (player.getPoints().count(Points::Victory)) {
        totalVP += player.getPoints().at(Points::Victory);
    }

    // 6. Wonders
    for (const auto& wonder : player.getWonders()) {
        if (wonder->getIsBuilt()) {
            // Wonder VPs are usually added to Points::Victory or handled here
        }
    }

    // 7. Guilds
    const Player& opponent = (player.getName() == m_player1->getName()) ? *m_player2 : *m_player1;
    totalVP += player.getVPFromGuilds(opponent);

    return totalVP;
}

std::optional<Player> Game::determinateWinner()
{
    // 1. Check for Military Supremacy
    int militaryPosition = m_board.getMilitaryTrack().getPawnPosition();
    if (militaryPosition >= MILITARY_VICTORY_THRESHOLD) {
        std::cout << "\n--- Victorie Militara! ---\n";
        std::cout << "Castigatorul este: " << m_player2->getName() << "\n";
        return *m_player2;
    }
    if (militaryPosition <= -MILITARY_VICTORY_THRESHOLD) {
        std::cout << "\n--- Victorie Militara! ---\n";
        std::cout << "Castigatorul este: " << m_player1->getName() << "\n";
        return *m_player1;
    }

    // 2. Check for Scientific Supremacy
    if (m_player1->getNrOfScientificSymbols() >= SCIENCE_VICTORY_THRESHOLD) {
        std::cout << "\n--- Victorie Stiintifica! ---\n";
        std::cout << "Castigatorul este: " << m_player1->getName() << "\n";
        return *m_player1;
    }
    if (m_player2->getNrOfScientificSymbols() >= SCIENCE_VICTORY_THRESHOLD) {
        std::cout << "\n--- Victorie Stiintifica! ---\n";
        std::cout << "Castigatorul este: " << m_player2->getName() << "\n";
        return *m_player2;
    }

    // 3. If no instant winner, calculate Civilian (VP) victory
    int vp1 = calculatePlayerVP(*m_player1);
    int vp2 = calculatePlayerVP(*m_player2);

	std::cout << "\n--- Rezultatele Finale (Victorie Civila) ---\n";
    std::cout << m_player1->getName() << " Total VP: " << vp1 << "\n";
    std::cout << m_player2->getName() << " Total VP: " << vp2 << "\n";
    if (vp1 > vp2) {
        std::cout << "\n Câștigătorul este: " << m_player1->getName() << "\n";
        return *m_player1;
    }
    else if (vp2 > vp1) {
        std::cout << "\n Câștigătorul este: " << m_player2->getName() << "\n";
        return *m_player2;
    }
    else {
        // Tie-breaker: Blue cards VP
        int blueVP1 = m_player1->getVPFromBlueCards();
        int blueVP2 = m_player2->getVPFromBlueCards();

        if (blueVP1 > blueVP1) {
            std::cout << "Jocul s-a terminat la egalitate, dar " << m_player1->getName()
                << " castiga la tie-breaker (Clădiri Civile - Albastre).\n";
            return *m_player1;
        }
        else if (blueVP2 > blueVP1) {
            std::cout << "Jocul s-a terminat la egalitate, dar " << m_player2->getName()
                << " castiga la tie-breaker (Clădiri Civile - Albastre).\n";
            return *m_player2;
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
            if (node && node->getCard() && node->getCard()->getId() == cardId) {
                selectedNode = const_cast<CardNode*>(node.get());
            }
        }
    }

    if (!selectedNode) {
        std::cout << "Eroare: Nu exista carte cu ID-ul " << cardId << "! Incearca iar.\n";
        handlePlayerAction();
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

    std::cout << "Ai ales: " << selectedNode->getCard()->getName() << "\n";
    std::cout << "Ce faci? [1]=Cumpara, [2]=Vinde, [3]=Construieste minune: ";
    int action;
    std::cin >> action;

    bool success = false;

    if (action == 1) {
        success = m_currentPlayer->buyCard(selectedNode->getCard(), *m_opponent, m_board);
    }
    else if (action == 2) {
        m_currentPlayer->discardCard(selectedNode->getCard(), m_board);
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
            m_currentPlayer->setHasExtraTurn(false);
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

    std::cout << "[P1] " << m_player1->getName()
        << " | Bani: " << (int)m_player1->getCoins()
        << " | Minuni: " << m_player1->getWonders().size() << "\n";

    std::cout << "[P2] " << m_player2->getName()
        << " | Bani: " << (int)m_player2->getCoins()
        << " | Minuni: " << m_player2->getWonders().size() << "\n";

    std::cout << "\n--- Pista Militara ---\n";
    m_board.printMilitaryTrack();

    m_board.printCardsTree(std::cout);
    std::cout << "============================================\n";
}

void Game::draftWondersPhase() {
    m_draftPhase = 1;
    m_draftSet1 = m_setup.drawWonders(4);
    m_draftSet2 = m_setup.drawWonders(4);

    m_currentPlayer = m_player1.get();
    m_opponent = m_player2.get();
    std::cout << "[Draft] Phase 1 Started. First player: " << m_currentPlayer->getName() << "\n";
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
    m_player1->addResource(Coin,7);
    m_player2->addResource(Coin, 7);
    m_board.updateVisibility();

    // --- END TEMPORARY TEST CODE ---
}

bool Game::checkForInstantWin() {

    int militaryPosition = m_board.getMilitaryTrack().getPawnPosition();

    if (militaryPosition >= 9) {
        m_gameOver = true;
        return true;
    }

    if (militaryPosition <=- 9) {
        m_gameOver = true;
        return true;
    }

	if (m_player1->getNrOfScientificSymbols() >= 6)
	{
        m_gameOver = true;
        return true;
    }

    if (m_player2->getNrOfScientificSymbols() >= 6) {
        m_gameOver = true;
        return true;
    }

    return false;
}

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
        return false; 
    }

    std::cout << "[Draft] " << m_currentPlayer->getName() << " (ptr: " << m_currentPlayer << ") drafts wonder ID " << wonderId << "\n";
    std::cout << "        P1: " << m_player1.get() << ", P2: " << m_player2.get() << "\n";
    m_currentPlayer->addWonders(*it);
    currentDraftSet.erase(it); 

    bool shouldSwitch = (currentDraftSet.size() == 3 || currentDraftSet.size() == 1);

    if (shouldSwitch) {
        switchTurn();
    }

    if (currentDraftSet.empty()) {
        m_draftPhase++; 
        if (m_draftPhase == 2) {
            // Phase 2 starts with Player 2
            m_currentPlayer = m_player2.get();
            m_opponent = m_player1.get();
            std::cout << "[Draft] Phase 2 Started. First player: " << m_currentPlayer->getName() << "\n";
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

bool Game::isWaitingForDiscardChoice() const
{
    return m_isWaitingForDiscardChoice;
}

const std::vector<std::shared_ptr<CardBase>>& Game::getBuildFromDiscardChoices() const
{
    return m_buildFromDiscardChoices;
}

void Game::handleBuildFromDiscard()
{
    const auto& discardPile = m_board.getDiscardPile();
    if (discardPile.empty()) {
        return;
    }

    m_buildFromDiscardChoices = discardPile;
    m_isWaitingForDiscardChoice = true;
}

void Game::resolveBuildFromDiscard(int chosenCardId)
{
    if (!m_isWaitingForDiscardChoice) {
        return;
    }

    std::shared_ptr<CardBase> chosenCard = nullptr;
    for (const auto& card : m_buildFromDiscardChoices) {
        if (card->getId() == chosenCardId) {
            chosenCard = card;
            break;
        }
    }

    if (chosenCard) {
        // Build the card for free
        m_currentPlayer->addCardToInventory(chosenCard);
        chosenCard->applyEffect(*m_currentPlayer, m_board, *m_opponent);
        m_board.removeCardFromDiscardPile(chosenCard);
    }

    // Reset state
    m_isWaitingForDiscardChoice = false;
    m_buildFromDiscardChoices.clear();
}



const std::vector<std::shared_ptr<CardBase>>& Game::getDiscardedCards() const
{
    return m_board.getDiscardPile();
}