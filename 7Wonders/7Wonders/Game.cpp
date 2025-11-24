#include "Game.h"

Game::Game() 
    : m_player1("Player 1"), 
      m_player2("Player 2"), 
      m_currentAge(1), 
      m_gameOver(false),
      m_setup(m_board)
{
    m_currentPlayer = &m_player1;
    m_opponent = &m_player2;
}

void Game::switchTurn()
{
	std::swap(m_currentPlayer, m_opponent);
    //era ceva minune de iti mai da o tura dar inca nu stiu cum sa o aplic
}

void Game::startNextAge()
{
    m_currentAge++;
    if (m_currentAge > 3) {
        //sa apelam aici o functie de ne calculeaza cine a castigat?
        m_gameOver = true;
        return;
    }
	//trebuie ceva ca sa vedem cine incepe primul in urmatoarea epoca
	m_setup.startAge(m_currentAge);
}

bool Game::isEndOfAge()
{
    return m_setup.getBoard().isPyramidEmpty();
}
