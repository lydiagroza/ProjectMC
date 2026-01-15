#include "GameSetup.h"
#include "UniversalCardLoader.h"
#include "ProgressTokenLoader.h"
#include <iostream>
#include <random>
#include <chrono>
#include "Game.h"

GameSetup::GameSetup(Board& board) : m_board(board) 
{
    loadAllResources();
	prepareTokens();
    prepareDecks();   
}

void GameSetup::loadAllResources()
{
    m_allTokens = ProgressTokenLoader::loadProgressTokens("ProgressTokens.csv");

    m_deckAge1 = UniversalCardLoader::loadAgeCards("AgeI.csv");
    m_deckAge2 = UniversalCardLoader::loadAgeCards ("AgeII.csv");
    m_deckAge3 = UniversalCardLoader::loadAgeCards("AgeIII.csv");
    m_deckGuilds = UniversalCardLoader::loadGuilds("Guilds.csv");
    
    m_allWonders = UniversalCardLoader::loadWonders("Wonders.csv");
}


void GameSetup::prepareTokens()
{
    if (m_allTokens.empty()) 
        return;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);

    std::shuffle(m_allTokens.begin(), m_allTokens.end(), e);

    std::vector<std::shared_ptr<ProgressToken>> selectedTokens;
    for (size_t i = 0; i < 5 && i < m_allTokens.size(); ++i)
        selectedTokens.push_back(m_allTokens[i]);

    m_board.setAvailableProgressTokens(selectedTokens);
}

void GameSetup::prepareDecks()
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);

    //era 1
    std::shuffle(m_deckAge1.begin(), m_deckAge1.end(), e);
    if (m_deckAge1.size() > 20) 
        m_deckAge1.resize(20); 

    //era 2
    std::shuffle(m_deckAge2.begin(), m_deckAge2.end(), e);
    if (m_deckAge2.size() > 20)
        m_deckAge2.resize(20);

    //era 3
    std::shuffle(m_deckGuilds.begin(), m_deckGuilds.end(), e);
    if (m_deckGuilds.size() > 3)
        m_deckGuilds.resize(3);
    m_deckAge3.insert(m_deckAge3.end(), m_deckGuilds.begin(), m_deckGuilds.end());
    std::shuffle(m_deckAge3.begin(), m_deckAge3.end(), e);
    if (m_deckAge3.size() > 20) 
        m_deckAge3.resize(20);

	//facem suffle si la minuni
    std::shuffle(m_allWonders.begin(), m_allWonders.end(), e);

}

GameSetup& GameSetup::startAge(int age)
{

    if (age == 1) {
        prepareTokens();
        m_board.setupCards(1, m_deckAge1);
    }
    else if (age == 2)
        m_board.setupCards(2, m_deckAge2);
    else if (age == 3)
        m_board.setupCards(3, m_deckAge3);

    return *this;
}

Board& GameSetup::getBoard()
{
    return m_board;
}

void GameSetup::printDecks() const
{
    std::cout << "--- Age 1 Deck ---" << std::endl;
    for (const auto& card : m_deckAge1)
        if (card) 
            std::cout << *card << std::endl; 

    std::cout << "\n--- Age 2 Deck ---" << std::endl;
    for (const auto& card : m_deckAge2)
        if (card) 
            std::cout << *card << std::endl; 
    std::cout << "\n--- Age 3 Deck ---" << std::endl;
    for (const auto& card : m_deckAge3)
        if (card) 
            std::cout << *card << std::endl;
    std::cout << "\n--- Guilds Deck ---" << std::endl;
    for (const auto& card : m_deckGuilds)
        if (card)
            std::cout << *card << std::endl;
}

void GameSetup::printWonders() const
{
    std::cout << "\n--- All Wonders ---" << std::endl;
    for (const auto& wonder : m_allWonders)
        if (wonder) 
            std::cout << wonder->getName() << std::endl;
}

std::vector<std::shared_ptr<Wonder>> GameSetup::drawWonders(int count)
{
    int actualCount = std::min((int)m_allWonders.size(), count);
    std::vector<std::shared_ptr<Wonder>> selectedWonders;
    selectedWonders.insert(selectedWonders.end(),
        m_allWonders.begin(),
        m_allWonders.begin() + actualCount);

    //le sterg din pachetul cu toate
    m_allWonders.erase(m_allWonders.begin(), m_allWonders.begin() + actualCount);

    return selectedWonders;

}
