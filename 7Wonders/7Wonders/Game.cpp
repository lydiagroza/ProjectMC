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

void Game::printPlayerInfo(const Player& player, std::ostream& os) const
{
    os << player.getName() << "\n";
}

void Game::switchTurn()
{
	std::swap(m_currentPlayer, m_opponent);
    //era ceva minune de iti mai da o tura dar inca nu stiu cum sa o aplic
}

void Game::checkForInstantWin(const MilitaryResult& militaryResult)
{
   /* if (militaryResult.supremacyAchieved) {
        std::cout << "\n!!! VICTORIE INSTANTANEE !!!\n";
        std::cout << m_currentPlayer->getName() << " a castigat prin SUPREMATIE MILITARA!\n";
        m_gameOver = true;
        return;
    }*/

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

void Game::run()
{
    // ... (logica de început de joc)
    

    for (m_currentAge = 1; m_currentAge <= 3 && !m_gameOver; ++m_currentAge)
    {
		m_setup.startAge(m_currentAge);
        std::cout << m_board;
        m_board.printChildrenList();

       /* while (!isEndOfAge() && !m_gameOver)
        {
            m_board.updateVisibility(); // Actualizează cărțile vizibile

             --- AFIȘEAZĂ TABLA AICI ---
           
            
             Afișează și starea jucătorilor, dacă ai o funcție pentru asta
            // std::cout << *m_currentPlayer;
            // std::cout << *m_opponent;

            //handlePlayerAction();
            
            //checkForInstantWin(); // Verifică dacă tura curentă a dus la o victorie
            
            if (!m_gameOver) {
                switchTurn();
            }
        }*/
        
        // Aici poți adăuga logica de la finalul epocii (ex: conflict militar)
    }

    // ... (logica de final de joc)
}
