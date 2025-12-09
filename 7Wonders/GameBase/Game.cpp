#include "Game.h"
#include "MilitaryTrack.h"
#include <iostream>

Game* Game::currentGame = nullptr;

Game::Game()
    : m_player1("Player 1"),
    m_player2("Player 2"),
    m_currentAge(1),
    m_gameOver(false),
    m_setup(m_board)
{
    currentGame = this;
    m_currentPlayer = &m_player1;
    m_opponent = &m_player2;
}

// You can keep this helper, but eventually, the UI will read the name directly.
void Game::printPlayerInfo(const Player& player, std::ostream& os) const
{
    os << player.getName() << "\n";
}

void Game::switchTurn()
{
    std::swap(m_currentPlayer, m_opponent);
}

void Game::checkForInstantWin(const MilitaryResult& militaryResult)
{
    /* Logic remains commented out until you implement it */
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

// --- THE CHANGED FUNCTION ---
void Game::initialize()
{
    // 1. Reset variables
    m_currentAge = 1;
    m_gameOver = false;

    // 2. Prepare the board for the first age
    m_setup.startAge(m_currentAge);

    // 3. (Optional) Debug print to verify it loaded
    std::cout << "Game Initialized. Age 1 started." << std::endl;

    // 4. Return immediately! 
    // The MainWindow will now display m_board on the screen.
}