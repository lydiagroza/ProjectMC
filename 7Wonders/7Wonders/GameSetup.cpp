#include "GameSetup.h"
#include "CardLoader.h"
#include "WonderLoader.h"
#include "ProgressTokenLoader.h"
#include <iostream>
#include <random>
#include <chrono>


GameSetup::GameSetup(Board& board) : m_board(board) // <-- Inițializează referința aici
{
    // Corrected order of initialization
    loadAllResources(); // 1. Load all assets from files
    prepareDecks();     // 2. Prepare the card decks for all ages
   // prepareTokens();    // 3. Prepare and place the tokens on the board
}

void GameSetup::loadAllResources()
{
    m_allTokens = ProgressTokenLoader::loadProgressTokens("ProgressTokens.csv");

    m_deckAge1 = CardLoader::loadFromCSV("AgeI.csv");
    m_deckAge2 = CardLoader::loadFromCSV("AgeII.csv");
    m_deckAge3 = CardLoader::loadFromCSV("AgeIII.csv");
    m_deckGuilds = CardLoader::loadFromCSV("Guilds.csv");
    
    m_allWonders = WonderLoader::loadWonders("Wonders.csv");
}

std::vector<CardBase*> GameSetup::toRawPointerVector(const std::vector<std::shared_ptr<CardBase>>& sharedDeck)
{
    std::vector<CardBase*> rawDeck;
    rawDeck.reserve(sharedDeck.size());
    for (const auto& cardPtr : sharedDeck) {
        rawDeck.push_back(cardPtr.get()); // .get() returneaza pointerul brut
    }
    return rawDeck;
}
//aparent cica nu avem voie sa dam shared ptr-uri la board, deci trebuie sa convertim in raw ptr


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
}

void GameSetup::startAge(int age)
{

    if (age == 1) {
        prepareTokens();
        auto rawDeck = toRawPointerVector(m_deckAge1);
        m_board.setupCards(1, rawDeck);
    }
    else if (age == 2) {
        auto rawDeck = toRawPointerVector(m_deckAge2);
        m_board.setupCards(2, rawDeck);
    }
    else if (age == 3) {
        auto rawDeck = toRawPointerVector(m_deckAge3);
        m_board.setupCards(3, rawDeck);
    }
}

Board& GameSetup::getBoard()
{
    return m_board;
}

void GameSetup::printDecks() const
{
    std::cout << "--- Age 1 Deck ---" << std::endl;
    for (const auto& card : m_deckAge1)
    {
        if (card) {
            std::cout << *card << std::endl; // Use the operator<<
        }
    }

    std::cout << "\n--- Age 2 Deck ---" << std::endl;
    for (const auto& card : m_deckAge2)
    {
        if (card) {
            std::cout << *card << std::endl; // Use the operator<<
        }
    }

    std::cout << "\n--- Age 3 Deck ---" << std::endl;
    for (const auto& card : m_deckAge3)
    {
        if (card) {
            std::cout << *card << std::endl; // Use the operator<<
        }
    }

    std::cout << "\n--- Guilds Deck ---" << std::endl;
    for (const auto& card : m_deckGuilds)
    {
        if (card) {
            std::cout << *card << std::endl; // Use the operator<<
        }
    }
}

void GameSetup::printWonders() const
{
    std::cout << "\n--- All Wonders ---" << std::endl;
    for (const auto& wonder : m_allWonders)
    {
        if (wonder) // Good practice to check if the pointer is valid
        {
            std::cout << wonder->getName() << std::endl;
        }
    }
}
