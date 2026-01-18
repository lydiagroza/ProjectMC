#include "SaveManager.h"
#include "UniversalCardLoader.h"
#include "ProgressTokenLoader.h"
#include "GameSetup.h" 
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

static std::string resolvePath(const std::string& filename) {
    std::vector<std::string> searchPaths = {
        "GameBase/",
        "../GameBase/",
        "../../GameBase/",
        "../../../GameBase/",
        "./"
    };
    for (const auto& prefix : searchPaths) {
        std::string fullPath = prefix + filename;
        if (fs::exists(fullPath)) return fullPath;
    }
    return filename;
}

SaveManager::GameRegistry SaveManager::buildRegistry() {
    GameRegistry reg;
    
    // Load Cards
    auto age1 = UniversalCardLoader::loadAgeCards(resolvePath("AgeI.csv"));
    auto age2 = UniversalCardLoader::loadAgeCards(resolvePath("AgeII.csv"));
    auto age3 = UniversalCardLoader::loadAgeCards(resolvePath("AgeIII.csv"));
    auto guilds = UniversalCardLoader::loadGuilds(resolvePath("Guilds.csv"));

    auto addToReg = [&](const std::vector<std::shared_ptr<CardBase>>& cards) {
        for(auto& c : cards) {
            if(c) reg.cards[c->getId()] = c;
        }
    };
    addToReg(age1);
    addToReg(age2);
    addToReg(age3);
    addToReg(guilds);

    // Load Wonders
    auto wonders = UniversalCardLoader::loadWonders(resolvePath("Wonders.csv"));
    for(auto& w : wonders) {
        if(w) reg.wonders[w->getId()] = w;
    }

    // Load Tokens
    auto tokens = ProgressTokenLoader::loadProgressTokens(resolvePath("ProgressTokens.csv"));
    for(auto& t : tokens) {
        if(t) reg.tokens[t->getId()] = t;
    }

    return reg;
}

bool SaveManager::saveGame(Game* game, const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_0);

    out << QString("7WD_SAVE");
    out << (qint32)1; 

    out << (qint32)game->m_currentAge;
    out << (qint32)game->m_draftPhase;
    out << game->m_gameOver;
    out << (qint32)game->m_numberOfWondersBuilt;
    
    int currentPlayerId = (game->m_currentPlayer == game->m_player1.get()) ? 1 : 2;
    out << (qint32)currentPlayerId;

    out << game->m_isWaitingForDiscardChoice;
    out << game->m_isWaitingForProgressTokenChoice;
    out << game->m_isWaitingForGreatLibraryChoice;
    out << game->m_isWaitingForOpponentCardDiscard;

    auto serializeWonderList = [&](const std::vector<std::shared_ptr<Wonder>>& list) {
        out << (qint32)list.size();
        for(const auto& w : list) {
            out << (qint32)w->getId();
        }
    };
    serializeWonderList(game->m_draftSet1);
    serializeWonderList(game->m_draftSet2);

    serializePlayer(out, *game->m_player1);
    serializePlayer(out, *game->m_player2);

    serializeBoard(out, game->m_board);

    return true;
}

Game* SaveManager::loadGame(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return nullptr;
    }
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_0);

    QString header;
    qint32 version;
    in >> header >> version;

    if (header != "7WD_SAVE") return nullptr;

    Game* game = new Game(); 
    GameRegistry reg = buildRegistry();

    qint32 age, draftPhase, numWonders, cpId;
    bool gameOver;
    in >> age >> draftPhase >> gameOver >> numWonders >> cpId;

    game->m_currentAge = age;
    game->m_draftPhase = draftPhase;
    game->m_gameOver = gameOver;
    game->m_numberOfWondersBuilt = numWonders;
    
    in >> game->m_isWaitingForDiscardChoice;
    in >> game->m_isWaitingForProgressTokenChoice;
    in >> game->m_isWaitingForGreatLibraryChoice;
    in >> game->m_isWaitingForOpponentCardDiscard;

    auto deserializeWonderList = [&](std::vector<std::shared_ptr<Wonder>>& list) {
        qint32 size;
        in >> size;
        list.clear();
        for(int i=0; i<size; ++i) {
            qint32 id;
            in >> id;
            if (reg.wonders.count(id)) {
                list.push_back(reg.wonders.at(id));
            }
        }
    };
    deserializeWonderList(game->m_draftSet1);
    deserializeWonderList(game->m_draftSet2);

    deserializePlayer(in, *game->m_player1, reg);
    deserializePlayer(in, *game->m_player2, reg);

    if (cpId == 1) {
        game->m_currentPlayer = game->m_player1.get();
        game->m_opponent = game->m_player2.get();
    } else {
        game->m_currentPlayer = game->m_player2.get();
        game->m_opponent = game->m_player1.get();
    }

    deserializeBoard(in, game->m_board, reg, game->m_currentAge);
    
    return game;
}

void SaveManager::serializePlayer(QDataStream& out, const Player& p) {
    out << QString::fromStdString(p.getName());
    
    out << (qint32)p.m_Resources.size();
    for(auto const& [res, val] : p.m_Resources) {
        out << (qint32)res << (qint32)val;
    }

    out << (qint32)p.m_pointsScore.size();
    for(auto const& [pt, val] : p.m_pointsScore) {
        out << (qint32)pt << (qint32)val;
    }

    out << (qint32)p.m_Wonders.size();
    for(const auto& w : p.m_Wonders) {
        out << (qint32)w->getId();
        out << w->getIsBuilt(); 
    }

    out << (qint32)p.m_Inventory.size();
    for(auto const& [col, list] : p.m_Inventory) {
        out << (qint32)col;
        out << (qint32)list.size();
        for(const auto& c : list) {
            out << (qint32)c->getId();
        }
    }

    out << (qint32)p.m_progressTokens.size();
    for(const auto& t : p.m_progressTokens) {
        out << (qint32)t->getId();
    }

    out << (qint32)p.m_scientificSymbols.size();
    for(auto const& [sym, val] : p.m_scientificSymbols) {
        out << (qint32)sym << (qint32)val;
    }

    out << (quint64)p.m_flags.to_ulong();
    
    out << (qint32)p.m_chainSymbols.size();
    for(const auto& sym : p.m_chainSymbols) {
        out << (qint32)sym;
    }
}

void SaveManager::deserializePlayer(QDataStream& in, Player& p, const GameRegistry& reg) {
    QString name;
    in >> name;
    p.m_name = name.toStdString();

    qint32 size;
    in >> size;
    p.m_Resources.clear();
    for(int i=0; i<size; ++i) {
        qint32 r, v;
        in >> r >> v;
        p.m_Resources[(Resource)r] = (uint8_t)v;
    }

    in >> size;
    p.m_pointsScore.clear();
    for(int i=0; i<size; ++i) {
        qint32 pt, v;
        in >> pt >> v;
        p.m_pointsScore[(Points)pt] = (uint8_t)v;
    }

    in >> size;
    p.m_Wonders.clear();
    for(int i=0; i<size; ++i) {
        qint32 id; 
        bool isBuilt;
        in >> id >> isBuilt;
        if(reg.wonders.count(id)) {
            auto w = reg.wonders.at(id); 
            if(isBuilt) w->setIsBuilt(); 
            p.m_Wonders.push_back(w);
        }
    }

    in >> size;
    p.m_Inventory.clear();
    for(int i=0; i<size; ++i) {
        qint32 col, count;
        in >> col >> count;
        std::vector<std::shared_ptr<CardBase>> list;
        for(int j=0; j<count; ++j) {
            qint32 id;
            in >> id;
            if(reg.cards.count(id)) {
                list.push_back(reg.cards.at(id));
            }
        }
        p.m_Inventory[(Color)col] = list;
    }

    in >> size;
    p.m_progressTokens.clear();
    for(int i=0; i<size; ++i) {
        qint32 id;
        in >> id;
        if(reg.tokens.count(id)) {
            p.m_progressTokens.push_back(reg.tokens.at(id));
        }
    }

    in >> size;
    p.m_scientificSymbols.clear();
    for(int i=0; i<size; ++i) {
        qint32 sym, val;
        in >> sym >> val;
        p.m_scientificSymbols[(Scientific_Symbol)sym] = val;
    }

    quint64 flagsVal;
    in >> flagsVal;
    p.m_flags = std::bitset<8>(flagsVal);

    in >> size;
    p.m_chainSymbols.clear();
    for(int i=0; i<size; ++i) {
        qint32 sym;
        in >> sym;
        p.m_chainSymbols.insert((Symbol)sym);
    }
}

void SaveManager::serializeBoard(QDataStream& out, const Board& b) {
    out << (qint32)b.m_militaryTrack.pawnPosition;

    out << (qint32)b.m_discardPile.size();
    for(const auto& c : b.m_discardPile) {
        out << (qint32)c->getId();
    }

    out << (qint32)b.m_availableProgressTokens.size();
    for(const auto& t : b.m_availableProgressTokens) {
        out << (qint32)t->getId();
    }

    out << (qint32)b.m_removedProgressTokens.size();
    for(const auto& t : b.m_removedProgressTokens) {
        out << (qint32)t->getId();
    }
    
    const auto& rows = b.getPyramid().getRows();
    out << (qint32)rows.size();
    for(const auto& row : rows) {
        out << (qint32)row.size();
        for(const auto& node : row) {
            bool hasCard = (node->getCard() != nullptr);
            out << hasCard;
            if(hasCard) {
                out << (qint32)node->getCard()->getId();
                out << (bool)node->isPlayed(); 
                out << (qint32)node->getFace();
            }
        }
    }
}

void SaveManager::deserializeBoard(QDataStream& in, Board& b, const GameRegistry& reg, int age) {
    qint32 pawnPos;
    in >> pawnPos;
    b.m_militaryTrack.pawnPosition = pawnPos;

    qint32 size;
    in >> size;
    b.m_discardPile.clear();
    for(int i=0; i<size; ++i) {
        qint32 id;
        in >> id;
        if(reg.cards.count(id)) b.m_discardPile.push_back(reg.cards.at(id));
    }

    in >> size;
    b.m_availableProgressTokens.clear();
    for(int i=0; i<size; ++i) {
        qint32 id;
        in >> id;
        if(reg.tokens.count(id)) b.m_availableProgressTokens.push_back(reg.tokens.at(id));
    }

    in >> size;
    b.m_removedProgressTokens.clear();
    for(int i=0; i<size; ++i) {
        qint32 id;
        in >> id;
        if(reg.tokens.count(id)) b.m_removedProgressTokens.push_back(reg.tokens.at(id));
    }

    // Pyramid Structure
    qint32 numRows;
    in >> numRows;

    // Build Dummy Structure
    std::vector<std::shared_ptr<CardBase>> dummyDeck(30, nullptr); // Enough for any age
    b.m_pyramid.build(age, dummyDeck);

    // Overwrite with real data
    const auto& rows = b.getPyramid().getRows();
    
    if((size_t)numRows != rows.size()) {

        return; 
    }

    for(const auto& row : rows) {
        qint32 rowSize;
        in >> rowSize;
        for(const auto& node : row) {
            bool hasCard;
            in >> hasCard;
            if(hasCard) {
                qint32 id;
                bool isPlayed;
                qint32 face;
                in >> id >> isPlayed >> face;
                
                if(reg.cards.count(id)) {
                    node->setCard(reg.cards.at(id));
                }
                node->updatePlayedStatus(isPlayed);
                node->setFace((Face)face);
            }
        }
    }
    
    b.updateVisibility();
}
