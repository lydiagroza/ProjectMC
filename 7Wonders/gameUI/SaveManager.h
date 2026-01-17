#pragma once
#include <QString>
#include <QDataStream>
#include <map>
#include <memory>
#include "Game.h"
#include "CardBase.h"
#include "Wonder.h"
#include "ProgressTokens.h"

class SaveManager {
public:
    static bool saveGame(Game* game, const QString& filePath);
    static Game* loadGame(const QString& filePath);

private:
    // Global Registry for ID resolution during loading
    struct GameRegistry {
        std::map<int, std::shared_ptr<CardBase>> cards;
        std::map<int, std::shared_ptr<Wonder>> wonders;
        std::map<int, std::shared_ptr<ProgressToken>> tokens;
    };

    static GameRegistry buildRegistry();

    // Serialization Helpers
    static void serializeBoard(QDataStream& out, const Board& board);
    static void deserializeBoard(QDataStream& in, Board& board, const GameRegistry& registry, int age);

    static void serializePlayer(QDataStream& out, const Player& player);
    static void deserializePlayer(QDataStream& in, Player& player, const GameRegistry& registry);
    
    static void serializeCardNode(QDataStream& out, const CardNode& node);
    static void deserializeCardNode(QDataStream& in, CardNode& node, const GameRegistry& registry);
};
