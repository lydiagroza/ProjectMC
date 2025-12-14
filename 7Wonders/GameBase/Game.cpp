#include "Game.h"
#include "MilitaryTrack.h"
#include <iostream>
#include <optional> // Add this include

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

    for (const Wonder& wonder : m_currentPlayer->getWonders())
        if (!wonder.getIsBuilt()) 
            availableWonders.push_back(const_cast<Wonder*>(&wonder));
       

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
        for (auto wonders : p.getWonders()) {

            if (!wonders.getIsBuilt() && wonders.getIsAvailable()) {
                return &wonders;
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
    totalVP += player.getCoins() / 3;
    totalVP += m_board.getMilitaryTrack().getVictoryPointsForPlayer(playerId);
   // totalVP += player.getVPFromBuildings();
    totalVP += player.getVPFromMilitaryTokens();
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
                // (Sau faci o funcție în Board care returnează Node* ne-const)
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

void handleWonderChoice( std::vector<std::shared_ptr<Wonder>>& availableWonders, Player& player, int count)
{
    for (int i = 0; i < count; ++i) {

        std::cout << "\n--- Minuni Disponibile Pt " << player.getName() << " (Alege #" << i + 1 << ") ---\n";
        for (size_t j = 0; j < availableWonders.size(); ++j) {
            const auto& wonder = availableWonders[j];
            std::cout << "[" << j + 1 << "] " << wonder->getName() << "\n";
        }

        std::cout << "Alege numarul Minunii (1-" << availableWonders.size() << "): ";
        int choiceIndex;
        std::cin >> choiceIndex;

        size_t index = choiceIndex - 1; // Calculăm indexul bazat pe 0

        if (index < availableWonders.size()) {

            // Transfer Minunea la Jucător (Apelezi funcția pe care am adăugat-o)
            player.addWonders(availableWonders[index]);
            std::cout << player.getName() << " a ales: " << availableWonders[index]->getName() << "\n";

            //Elimină Minunea din lista de opțiuni
            availableWonders.erase(availableWonders.begin() + index);

        }
        else {
            // Dacă input-ul este invalid, cerem alegerea din nou
            std::cout << "Alegere invalida. Incearca din nou.\n";
            i--;
        }
    }
}

void Game::draftWondersPhase() {
    Player* p1 = &m_player1;
    Player* p2 = &m_player2;
    std::cout << "\n\n=== [DRAFT 1] ALEGERE MINUNI (4) ===\n";
    std::vector<std::shared_ptr<Wonder>> draftSet1 = m_setup.drawWonders(4);

    handleWonderChoice(draftSet1, *p1, 1);
    handleWonderChoice(draftSet1, *p2, 2);

    if (!draftSet1.empty()) {
        std::cout << p1->getName() << " ia automat Minunea ramasa: " << draftSet1[0]->getName() << "\n";
        p1->addWonders(draftSet1[0]); 
    }

    std::cout << "\n\n=== [DRAFT 2] ALEGERE MINUNI (4) ===\n";
    std::vector<std::shared_ptr<Wonder>> draftSet2 = m_setup.drawWonders(4);

    handleWonderChoice(draftSet2, *p2, 1);
    handleWonderChoice(draftSet2, *p1, 2);

    if (!draftSet2.empty()) {
        std::cout << p2->getName() << " ia automat Minunea ramasa: " << draftSet2[0]->getName() << "\n";
        p2->addWonders(draftSet2[0]);
    }
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
    m_player1.addCoins(7);
    m_player2.addCoins(7);
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

	if (m_player1.getNrOfScientificSymbols() >= 6) { //trebuie implementata fct in player
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