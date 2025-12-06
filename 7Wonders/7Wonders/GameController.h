#pragma once

#include<QObject>
#include<memory>

class Player;
class Game;
class CardBase;
class Wonder;
class Board;

class GameController: public QObject {
	Q_OBJECT
public:
	explicit GameController(QObject* parent = nullptr);
	~GameController();

	void startNewGame();
private:
	Player* m_currentPlayer;
	Player* m_otherPlayer;
	std::shared_ptr<CardBase> m_selectedCard;
	int m_currentAge;
	Game* m_gameState;
signals:
	void gameStarted();
public slots:


};

