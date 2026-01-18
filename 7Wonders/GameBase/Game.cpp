#include "Game.h"
#include "MilitaryTrack.h"
#include "AI_Player.h"
#include "GuildEffects.h"
#include <iostream>
#include <optional> 
#include <vector>
#include <memory>
#include <algorithm>
#include <ranges>
#include <format>
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

    if (p1IsAI) {
        auto ai = std::make_shared<AI_Player>(n1, 1, difficulty);
        if (difficulty == AI_Difficulty::ADAPTIVE) ai->enableTraining(true);
        m_player1 = ai;
    }
    else m_player1 = std::make_shared<Player>(n1, 1);

    if (p2IsAI) {
        auto ai = std::make_shared<AI_Player>(n2, 2, difficulty);
        if (difficulty == AI_Difficulty::ADAPTIVE) ai->enableTraining(true);
        m_player2 = ai;
    }
    else m_player2 = std::make_shared<Player>(n2, 2);

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
    std::cout << std::format("[Turn] Switched to {} (ptr: {})\n", m_currentPlayer->getName(), static_cast<void*>(m_currentPlayer));
}

void Game::notifyWonderBuilt()
{
    m_numberOfWondersBuilt++;
    if (m_numberOfWondersBuilt == 7)
        handle7WondersRule();
}

bool Game::handleWonderConstruction(std::shared_ptr<CardBase> cardUsed) {

    std::vector<Wonder*> availableWonders;

    for (const auto& wonderPtr : m_currentPlayer->getWonders()) {
        if (wonderPtr && !wonderPtr->getIsBuilt() && wonderPtr->getIsAvailable()) {
            availableWonders.push_back(wonderPtr.get());
        }
    }

    if (availableWonders.empty()) {
        std::cout << "Error: You have no available unbuilt Wonders to build.\n";
        return false;
    }

    std::cout << "\n--- Available Unbuilt Wonders ---\n";
    for (size_t i = 0; i < availableWonders.size(); ++i)
        std::cout << "[" << i + 1 << "] "<< availableWonders[i]->getName()<< "\n";
   
    std::cout << "Choose the number of the Wonder you want to build: ";
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid choice. Please enter a number.\n";
        return false;
    }

    size_t index = choice - 1;

    if (index >= availableWonders.size()) {
        std::cout << "Invalid choice. Try again.\n";
        return false;
    }

    auto selectedWonder = availableWonders[index];

    m_currentPlayer->constructWonder(
        cardUsed,
        *selectedWonder,
        *m_opponent,
        m_board
    );

    if (selectedWonder->getIsBuilt()) {
        std::cout << m_currentPlayer->getName() << " has built the Wonder: " << selectedWonder->getName() << "\n";
        notifyWonderBuilt();
        return true;
    }
    return false;
}

void Game::handle7WondersRule()
{
	Wonder* lastRemainingWonder = nullptr;
    auto findUnbuiltAvailableWonder = [](Player& p) -> Wonder* {
        for (const auto& wonderPtr : p.getWonders()) {
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
        std::cout<< lastRemainingWonder->getName() << "' is removed from the game (returned to the box).\n";
        lastRemainingWonder->returnToBox();
    }
}

    
int Game::calculatePlayerVP(const Player& player) const {
    int totalVP = 0;
    int playerId = (player.getName() == m_player1->getName()) ? 1 : 2;
    
    totalVP += player.getCoins() / 3;
    
    totalVP += m_board.getMilitaryTrack().getVictoryPointsForPlayer(playerId);
    
    totalVP += player.getVPFromMilitaryTokens();

    totalVP += player.getVPFromBlueCards();

    if (player.getPoints().count(Points::Victory)) {
        totalVP += player.getPoints().at(Points::Victory);
    }

    for (const auto& wonder : player.getWonders()) {
        if (wonder->getIsBuilt()) {
        }
    }

    const Player& opponent = (player.getName() == m_player1->getName()) ? *m_player2 : *m_player1;
    totalVP += player.getVPFromGuilds(opponent);

    return totalVP;
}

std::optional<Player> Game::determinateWinner()
{
    int militaryPosition = m_board.getMilitaryTrack().getPawnPosition();
    if (militaryPosition >= MILITARY_VICTORY_THRESHOLD) {
        std::cout << "\n--- Military Victory! ---\n";
        std::cout << "The winner is: " << m_player2->getName() << "\n";
        return *m_player2;
    }
    if (militaryPosition <= -MILITARY_VICTORY_THRESHOLD) {
        std::cout << "\n--- Military Victory! ---\n";
        std::cout << "The winner is: " << m_player1->getName() << "\n";
        return *m_player1;
    }

    if (m_player1->getNrOfScientificSymbols() >= SCIENCE_VICTORY_THRESHOLD) {
        std::cout << "\n--- Scientific Victory! ---\n";
        std::cout << "The winner is: " << m_player1->getName() << "\n";
        return *m_player1;
    }
    if (m_player2->getNrOfScientificSymbols() >= SCIENCE_VICTORY_THRESHOLD) {
        std::cout << "\n--- Scientific Victory! ---\n";
        std::cout << "The winner is: " << m_player2->getName() << "\n";
        return *m_player2;
    }

    int vp1 = calculatePlayerVP(*m_player1);
    int vp2 = calculatePlayerVP(*m_player2);

	std::cout << "\n--- Final Results (Civilian Victory) ---\n";
    std::cout << m_player1->getName() << " Total VP: " << vp1 << "\n";
    
    auto printGuilds = [&](Player& p, Player& opp) {
        const auto& inv = p.getInventory();
        if (inv.count(Color::Purple)) {
            std::cout << "   -> Guild Details (" << p.getName() << "):\n";
            for (const auto& card : inv.at(Color::Purple)) {
                int pts = 0;
                int id = card->getId();
                switch (id) {
                    case 67: pts = GuildEndGameEffects::calculateBuildersGuild(p, opp); break;
                    case 68: pts = GuildEndGameEffects::calculateMoneylendersGuild(p, opp); break;
                    case 69: pts = GuildEndGameEffects::calculateScientistsGuild(p, opp); break;
                    case 70: pts = GuildEndGameEffects::calculateShipownersGuild(p, opp); break;
                    case 71: pts = GuildEndGameEffects::calculateTradersGuild(p, opp); break;
                    case 72: pts = GuildEndGameEffects::calculateMagistratesGuild(p, opp); break;
                    case 73: pts = GuildEndGameEffects::calculateTacticiansGuild(p, opp); break;
                }
                std::cout << "      - " << card->getName() << ": " << pts << " VP\n";
            }
        } else {
            std::cout << "   -> " << p.getName() << " has no guilds.\n";
        }
    };

    printGuilds(*m_player1, *m_player2);

    std::cout << m_player2->getName() << " Total VP: " << vp2 << "\n";
    printGuilds(*m_player2, *m_player1);

    if (vp1 > vp2) {
        std::cout << "\n The winner is: " << m_player1->getName() << "\n";
        return *m_player1;
    }
    else if (vp2 > vp1) {
        std::cout << "\n The winner is: " << m_player2->getName() << "\n";
        return *m_player2;
    }
    else {
        int blueVP1 = m_player1->getVPFromBlueCards();
        int blueVP2 = m_player2->getVPFromBlueCards();

        if (blueVP1 > blueVP2) {
            std::cout << "The game ended in a tie, but " << m_player1->getName()
                << " wins on the tie-breaker (Civilian Buildings - Blue).\n";
            return *m_player1;
        }
        else if (blueVP2 > blueVP1) {
            std::cout << "The game ended in a tie, but " << m_player2->getName()
                << " wins on the tie-breaker (Civilian Buildings - Blue).\n";
            return *m_player2;
        }
        else {
            std::cout << "The game ended in a perfect tie. It's a draw.\n";
            return std::nullopt;
        }
    }
}

void Game::handlePlayerAction()
{
    std::cout << "\n>>> IT'S YOUR TURN: " << m_currentPlayer->getName() << "\n";
    std::cout << "Resources (Coins): " << (int)m_currentPlayer->getCoins() << "\n";
    std::cout << "Enter the ID of the card you want to take: ";

    int cardId;
    while (!(std::cin >> cardId)) {
        if (std::cin.eof()) {
            std::cerr << "Input stream ended. Exiting.\n";
            exit(0);
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Please enter a valid number: ";
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
        std::cout << "Error: There is no card with ID " << cardId << "! Try again.\n";
        handlePlayerAction();
        return;
    }
    if (selectedNode->isPlayed()) {
        std::cout << "Error: The card has already been taken! Try again.\n";
        handlePlayerAction();
        return;
    }
    if (!selectedNode->isPlayable()) {
        std::cout << "Error: The card is blocked by others! Try again.\n";
        handlePlayerAction();
        return;
    }

    std::cout << "You chose: " << selectedNode->getCard()->getName() << "\n";
    std::cout << "What do you do? [1]=Buy, [2]=Sell, [3]=Build Wonder: ";
    int action;
    std::cin >> action;

    bool success = false;

    if (action == 1) {
        success = m_currentPlayer->buyCard(selectedNode->getCard(), *m_opponent, m_board);
    }
    else if (action == 2) {
        m_currentPlayer->discardCard(selectedNode->getCard(), m_board);
        std::cout << "Card sold for money.\n";
        success = true;
    }
    else if (action == 3)
    {
        success = handleWonderConstruction(selectedNode->getCard());
        if (success)
            selectedNode->updatePlayedStatus(true);
    }
    else {
        std::cout << "Invalid action.\n";
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
            std::cout << "You received an extra turn! You play again.\n";
            m_currentPlayer->setHasExtraTurn(false);
        }
    }
    else {
        std::cout << "The action failed. Try something else.\n";
        handlePlayerAction();
    }
}

void Game::printGameState() const {
    std::cout << "\n============================================\n";
    std::cout << "               GAME STATE (Age " << m_currentAge << ")      \n";
    std::cout << "============================================\n";

    std::cout << "[P1] " << m_player1->getName()
        << " | Coins: " << (int)m_player1->getCoins()
        << " | Wonders: " << m_player1->getWonders().size() << "\n";

    std::cout << "[P2] " << m_player2->getName()
        << " | Coins: " << (int)m_player2->getCoins()
        << " | Wonders: " << m_player2->getWonders().size() << "\n";

    std::cout << "\n--- Military Track ---\n";
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

    auto it = std::ranges::find_if(currentDraftSet,
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
        m_currentPlayer->addCardToInventory(chosenCard);
        chosenCard->applyEffect(*m_currentPlayer, m_board, *m_opponent);
        m_board.removeCardFromDiscardPile(chosenCard);
    }

    m_isWaitingForDiscardChoice = false;
    m_buildFromDiscardChoices.clear();
}



const std::vector<std::shared_ptr<CardBase>>& Game::getDiscardedCards() const
{
    return m_board.getDiscardPile();
}

bool Game::isWaitingForOpponentCardDiscard() const
{
    return m_isWaitingForOpponentCardDiscard;
}

const std::vector<std::shared_ptr<CardBase>>& Game::getOpponentCardDiscardChoices() const
{
    return m_opponentCardDiscardChoices;
}

void Game::handleDiscardOpponentCardChoice(Player& opponent, Color cardColor)
{
    m_opponentCardDiscardChoices.clear();
    const auto& opponentInventory = opponent.getInventory();
    if (opponentInventory.count(cardColor)) {
        m_opponentCardDiscardChoices = opponentInventory.at(cardColor);
    }

    if (m_opponentCardDiscardChoices.empty()) {
        return;
    }

    m_targetOpponent = &opponent;
    m_isWaitingForOpponentCardDiscard = true;
}

void Game::resolveDiscardOpponentCard(int chosenCardId)
{
    if (!m_isWaitingForOpponentCardDiscard || !m_targetOpponent) {
        return;
    }

    std::shared_ptr<CardBase> chosenCard = nullptr;
    for (const auto& card : m_opponentCardDiscardChoices) {
        if (card->getId() == chosenCardId) {
            chosenCard = card;
            break;
        }
    }

    if (chosenCard) {
        m_targetOpponent->removeCardFromInventory(chosenCard);
        m_board.addCardToDiscardPile(chosenCard);
        std::cout << "[Effect] " << m_currentPlayer->getName() << " discarded " << chosenCard->getName() << " from " << m_targetOpponent->getName() << "." << std::endl;
    }

    m_isWaitingForOpponentCardDiscard = false;
    m_opponentCardDiscardChoices.clear();
    m_targetOpponent = nullptr;
}