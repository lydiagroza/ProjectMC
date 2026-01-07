#include "Game.h"
#include "MilitaryTrack.h"
#include <iostream>
#include <optional>
#include <algorithm>

Game* Game::currentGame = nullptr;

Game::Game()
    : m_board(),
    m_setup(m_board),
    m_player1("Player 1", 1),
    m_player2("Player 2", 2),
    m_currentAge(1),
    m_gameOver(false),
    m_numberOfWondersBuilt(0)
{
    currentGame = this;
    m_currentPlayer = &m_player1;
    m_opponent = &m_player2;
}

void Game::initialize()
{
    m_currentAge = 1;
    m_gameOver = false;
    m_numberOfWondersBuilt = 0;
    draftWondersPhase(); // Faza initiala de alegere a minunilor 
    m_setup.startAge(m_currentAge);
    m_player1.addResource(Coin, 7); // Fiecare jucator incepe cu 7 monede 
    m_player2.addResource(Coin, 7);
    m_board.updateVisibility();
    std::cout << "[Game Logic] Initialization Complete. Age 1 Started. Players have 7 coins.\n";
}

void Game::printPlayerInfo(const Player& player, std::ostream& os) const
{
    os << player.getName() << " (ID: " << player.getId() << ")\n";
}

void Game::switchTurn()
{
    Player* temp = m_currentPlayer;
    m_currentPlayer = m_opponent;
    m_opponent = temp;
}

// Gestionarea constructiei unei Minuni 
bool Game::handleWonderConstruction(std::shared_ptr<CardBase> cardUsed) {
    std::vector<Wonder*> availableWonders;

    for (const auto& wonder : m_currentPlayer->getWonders())
        if (!wonder->getIsBuilt() && wonder->getIsAvailable())
            availableWonders.push_back(const_cast<Wonder*>(wonder.get()));

    if (availableWonders.empty()) {
        std::cout << "Eroare: Nu ai Minuni disponibile pentru a construi.\n";
        return false;
    }

    std::cout << "\n--- Minuni Neconstruite Disponibile ---\n";
    for (size_t i = 0; i < availableWonders.size(); ++i)
        std::cout << "[" << i + 1 << "] " << availableWonders[i]->getName() << "\n";

    std::cout << "Alege numarul Minunii pe care vrei sa o construiesti: ";
    int choice;
    if (!(std::cin >> choice) || choice < 1 || choice >(int)availableWonders.size()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Alegere invalida.\n";
        return false;
    }

    Wonder* selectedWonder = availableWonders[choice - 1];

    m_currentPlayer->constructWonder(cardUsed, *selectedWonder, *m_opponent, m_board);

    if (selectedWonder->getIsBuilt()) {
        std::cout << m_currentPlayer->getName() << " a construit Minunea: " << selectedWonder->getName() << "\n";
        m_numberOfWondersBuilt++;

        if (m_numberOfWondersBuilt == 7) {
            handle7WondersRule(); // Regula: maxim 7 minuni in total [cite: 283, 284]
        }
        return true;
    }
    return false;
}

void Game::handle7WondersRule()
{
    Wonder* lastRemainingWonder = nullptr;
    auto findUnbuiltAvailableWonder = [](Player& p) -> Wonder* {
        for (auto& wonderPtr : p.getWonders()) {
            if (!wonderPtr->getIsBuilt() && wonderPtr->getIsAvailable()) {
                return wonderPtr.get();
            }
        }
        return nullptr;
        };

    lastRemainingWonder = findUnbuiltAvailableWonder(m_player1);
    if (!lastRemainingWonder) {
        lastRemainingWonder = findUnbuiltAvailableWonder(m_player2);
    }

    if (lastRemainingWonder) {
        std::cout << "Minunea '" << lastRemainingWonder->getName() << "' este scoasa din joc.\n";
        lastRemainingWonder->returnToBox(); // Returnata la cutie conform regulamentului [cite: 284]
    }
}

//logica necesara pt actiunea unui jucator

CardNode* Game::findNodeById(int cardId) const {
    const auto& rows = m_board.getPyramid().getRows();
    for (const auto& row : rows) {
        for (const auto& node : row) {
            if (node && node->getCard() && node->getCard()->getId() == cardId) {
                return const_cast<CardNode*>(node.get());
            }
        }
    }
    return nullptr;
}

void Game::handlePlayerAction() {
    if (m_gameOver) return;

    bool actionProcessed = false;
    while (!actionProcessed) {
        std::cout << "\n>>> RANDUL LUI: " << m_currentPlayer->getName() << "\n";
        std::cout << "Bani: " << (int)m_currentPlayer->getCoins() << "\n";

        int cardId;
        std::cout << "ID carte: ";
        if (!(std::cin >> cardId)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        CardNode* selectedNode = findNodeById(cardId);

        if (!selectedNode || selectedNode->isPlayed() || !selectedNode->isPlayable()) {
            std::cout << "Eroare: Cartea nu este disponibila!\n";
            continue;
        }

        std::shared_ptr<CardBase> selectedCard = selectedNode->getCard();
        std::cout << "Ai ales: " << selectedCard->getName() << "\n";
        std::cout << "[1]Cumpara, [2]Vinde, [3]Minune: ";

        int action;
        std::cin >> action;

        bool success = false;
        if (action == 1) {
            success = m_currentPlayer->buyCard(selectedCard, *m_opponent, m_board);
            if (success) selectedCard->applyEffect(*m_currentPlayer, m_board, *m_opponent);
        }
        else if (action == 2) {
            m_currentPlayer->discardCard(*selectedCard);
            success = true;
        }
        else if (action == 3) {
            success = handleWonderConstruction(selectedCard);
        }

        if (success) {
            selectedNode->updatePlayedStatus(true);
            m_board.updateVisibility();
            if (checkForInstantWin()) return;

            processTurnTransition();
            actionProcessed = true;
        }
    }
}

void Game::processTurnTransition() {
    if (m_currentPlayer->hasExtraTurn()) {
        m_currentPlayer->setExtraTurn(false);
        printGameState();
        handlePlayerAction(); // Aici este permis deoarece este un flux nou de joc (Replay)
    }
    else {
        switchTurn();
    }
}

void Game::handleProgressTokenChoice() {
    auto availableTokens = m_board.getAvailableProgressTokens();
    if (availableTokens.empty()) return;

    std::cout << "\n--- PERECHE FORMATA! ALEGE UN JETON DE PROGRES ---\n";
    for (size_t i = 0; i < availableTokens.size(); ++i) {
        std::cout << "[" << i << "] " << availableTokens[i]->getName() << "\n";
    }

    int choice;
    std::cout << "Selectia ta: ";
    if (!(std::cin >> choice) || choice < 0 || choice >= (int)availableTokens.size()) {
        std::cin.clear();
        std::cin.ignore();
        std::cout << "Alegere invalida.\n";
        return;
    }

    auto selectedToken = availableTokens[choice];
    m_currentPlayer->addProgressToken(selectedToken);

    // Aplicam efectele imediate ale jetonului 
    for (auto& effect : selectedToken->getEffects()) {
        effect(*m_currentPlayer, *m_opponent);
    }

    m_board.removeAvailableProgressToken(selectedToken);
    std::cout << "Ai obtinut jetonul: " << selectedToken->getName() << "\n";
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

void handleWonderChoice(std::vector<std::shared_ptr<Wonder>>& availableWonders, Player& player, int count)
{
    for (int i = 0; i < count; ++i) {
        std::cout << "\n--- Minuni Disponibile Pt " << player.getName() << " (Alege #" << i + 1 << ") ---\n";
        for (size_t j = 0; j < availableWonders.size(); ++j) {
            std::cout << "[" << j + 1 << "] " << availableWonders[j]->getName() << "\n";
        }

        std::cout << "Alege numarul Minunii (1-" << availableWonders.size() << "): ";
        int choiceIndex;
        if (!(std::cin >> choiceIndex) || choiceIndex < 1 || choiceIndex >(int)availableWonders.size()) {
            std::cin.clear();
            std::cin.ignore();
            std::cout << "Alegere invalida. Incearca din nou.\n";
            i--;
            continue;
        }

        size_t index = choiceIndex - 1;
        player.addWonders(availableWonders[index]);
        std::cout << player.getName() << " a ales: " << availableWonders[index]->getName() << "\n";
        availableWonders.erase(availableWonders.begin() + index);
    }
}

void Game::draftWondersPhase() {
    std::cout << "\n\n=== [DRAFT 1] ALEGERE MINUNI (4) ===\n";
    std::vector<std::shared_ptr<Wonder>> draftSet1 = m_setup.drawWonders(4);

    handleWonderChoice(draftSet1, m_player1, 1);
    handleWonderChoice(draftSet1, m_player2, 2);

    if (!draftSet1.empty()) {
        std::cout << m_player1.getName() << " ia automat Minunea ramasa: " << draftSet1[0]->getName() << "\n";
        m_player1.addWonders(draftSet1[0]);
    }

    std::cout << "\n\n=== [DRAFT 2] ALEGERE MINUNI (4) ===\n";
    std::vector<std::shared_ptr<Wonder>> draftSet2 = m_setup.drawWonders(4);

    handleWonderChoice(draftSet2, m_player2, 1);
    handleWonderChoice(draftSet2, m_player1, 2);

    if (!draftSet2.empty()) {
        std::cout << m_player2.getName() << " ia automat Minunea ramasa: " << draftSet2[0]->getName() << "\n";
        m_player2.addWonders(draftSet2[0]);
    }
}

void Game::startNextAge()
{
    m_currentAge++;
    if (m_currentAge > 3) {
        m_gameOver = true;
        return;
    }
    determinateWhoStartsNextAge();
    m_setup.startAge(m_currentAge);
    m_board.updateVisibility();
}

bool Game::isEndOfAge()
{
    return m_board.isPyramidEmpty(); // Era se termina cand piramida e goala 
}

bool Game::isGameOver() const
{
    return m_gameOver;
}

bool Game::checkForInstantWin() {
    int militaryPosition = m_board.getMilitaryTrack().getPawnPosition();

    // Victorie prin Suprematie Militara (atingerea capitalei adverse la +/- 9) 
    if (militaryPosition >= MILITARY_VICTORY_THRESHOLD) {
        std::cout << "VICTORIE MILITARA! " << m_player1.getName() << " a cucerit capitala adversa!\n";
        m_gameOver = true;
        return true;
    }
    if (militaryPosition <= -MILITARY_VICTORY_THRESHOLD) {
        std::cout << "VICTORIE MILITARA! " << m_player2.getName() << " a cucerit capitala adversa!\n";
        m_gameOver = true;
        return true;
    }

    // Victorie prin Suprematie Stiintifica (6 simboluri diferite) 
    if (m_player1.getNrOfScientificSymbols() >= SCIENCE_VICTORY_THRESHOLD) {
        std::cout << "VICTORIE STIINTIFICA! " << m_player1.getName() << " a descoperit 6 simboluri diferite!\n";
        m_gameOver = true;
        return true;
    }
    if (m_player2.getNrOfScientificSymbols() >= SCIENCE_VICTORY_THRESHOLD) {
        std::cout << "VICTORIE STIINTIFICA! " << m_player2.getName() << " a descoperit 6 simboluri diferite!\n";
        m_gameOver = true;
        return true;
    }

    return false;
}


int Game::calculatePlayerVP(const Player& player) const {
    int totalFinal = 0;
    const auto& pointsMap = player.getPoints();

    // VP (verzi/galbene/minuni) + Military (jetoane) + BlueCards (civile)
    for (auto const& [type, value] : pointsMap) 
        totalFinal += value;
	//punctele date de cartile mov (gilde)
    totalFinal+= calculatePurpleGuilds(player);
    // Adăugăm punctele din Tezaur (Bani): 1 VP la fiecare 3 monede
    totalFinal += player.getCoins() / COINS_PER_VICTORY_POINT;

    // Adăugăm punctele de pe Pista Militară (unde a rămas pionul la final: 0, 2, 5 sau 10)
    int playerId = (player.getName() == m_player1.getName()) ? 1 : 2;
    totalFinal += m_board.getMilitaryTrack().getVictoryPointsForPlayer(playerId);

    return totalFinal;
}
int Game::calculatePurpleGuilds(const Player& player) const {
    const auto& inventory = player.getInventory();
    auto it = inventory.find(Color::Purple);
    if (it == inventory.end()) return 0;

    int totalPoints = 0;
    const Player& opponent = (player.getId() == m_player1.getId()) ? m_player2 : m_player1;

    for (const auto& card : it->second) {
        // static_cast folosind enum-ul tău global
        GuildType type = static_cast<GuildType>(card->getId());

        switch (type) {
        case GuildType::Traders:
            totalPoints += std::max(getCategoryCount(player, Color::Brown) + getCategoryCount(player, Color::Gray),
                getCategoryCount(opponent, Color::Brown) + getCategoryCount(opponent, Color::Gray));
            break;
        case GuildType::Craftsmens:
            totalPoints += 2 * std::max(getBuiltWondersCount(player), getBuiltWondersCount(opponent));
            break;
        case GuildType::Philosophers:
            totalPoints += std::max(getCategoryCount(player, Color::Green), getCategoryCount(opponent, Color::Green));
            break;
        case GuildType::Spies:
            totalPoints += std::max(getCategoryCount(player, Color::Red), getCategoryCount(opponent, Color::Red));
            break;
        case GuildType::Shipowners:
            totalPoints += std::max(getCategoryCount(player, Color::Blue), getCategoryCount(opponent, Color::Blue));
            break;
        default:
            break;
        }
    }
    return totalPoints;
}

int Game::getCategoryCount(const Player& player, Color color) const {
    auto it = player.getInventory().find(color);
    return (it != player.getInventory().end()) ? static_cast<int>(it->second.size()) : 0;
}

int Game::getBuiltWondersCount(const Player& player) const {
    int count = 0;
    for (const auto& wonder : player.getWonders()) {
        if (wonder->getIsBuilt()) {
            count++;
        }
    }
    return count;
}
std::optional<Player> Game::determinateWinner()
{
    int vp1 = calculatePlayerVP(m_player1);
    int vp2 = calculatePlayerVP(m_player2);

    std::cout << "\n--- Rezultatele Finale ---\n";
    std::cout << m_player1.getName() << " Total VP: " << vp1 << "\n";
    std::cout << m_player2.getName() << " Total VP: " << vp2 << "\n";

    if (vp1 > vp2) {
        std::cout << "\n Castigatorul este: " << m_player1.getName() << "\n";
        return m_player1;
    }
    else if (vp2 > vp1) {
        std::cout << "\n Castigatorul este: " << m_player2.getName() << "\n";
        return m_player2;
    }
    else {
        int blueVP1 = m_player1.getVPFromBlueCards();
        int blueVP2 = m_player2.getVPFromBlueCards();

        if (blueVP1 > blueVP2) {
            std::cout << "Egalitate la puncte, dar " << m_player1.getName() << " castiga la departajare (Cladiri Albastre).\n";
            return m_player1;
        }
        else if (blueVP2 > blueVP1) {
            std::cout << "Egalitate la puncte, dar " << m_player2.getName() << " castiga la departajare (Cladiri Albastre).\n";
            return m_player2;
        }
        else {
            std::cout << "Egalitate perfecta. Remiza!\n";
            return std::nullopt;
        }
    }
}

void Game::determinateWhoStartsNextAge()
{
    int militaryPos = m_board.getMilitaryTrack().getPawnPosition();
    Player* picker = nullptr;

    if (militaryPos > 0) {
        // Pionul e spre Player 1, deci Player 2 domină militar. P1 alege.
        picker = &m_player1;
    }
    else if (militaryPos < 0) {
        // Pionul e spre Player 2, deci Player 1 domină militar. P2 alege.
        picker = &m_player2;
    }
    else {
        // Egalitate militară: jucătorul care a jucat ultima carte alege cine începe
        // De regulă, în codul tău, m_currentPlayer este cel care tocmai a terminat.
        // Regulamentul spune: "The player who played the last card chooses".
        picker = m_currentPlayer;
    }

    std::cout << "\n--- TRANZITIE EON ---" << std::endl;
    std::cout << picker->getName() << " decide cine incepe Era " << m_currentAge << ".\n";
    std::cout << "1. Incep eu\n2. Incepe oponentul\nChoice: ";

    int choice;
    std::cin >> choice;

    if (choice == 1) {
        m_currentPlayer = picker;
        m_opponent = (picker == &m_player1) ? &m_player2 : &m_player1;
    }
    else {
        m_opponent = picker;
        m_currentPlayer = (picker == &m_player1) ? &m_player2 : &m_player1;
    }
}
