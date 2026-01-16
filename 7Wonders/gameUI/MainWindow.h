#pragma once
#include <QtWidgets/QMainWindow>
#include <QStackedWidget>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout> 
#include "Game.h"
#include "BoardWidget.h"
#include "WonderSelectionWidget.h" 
#include "MilitaryTrackWidget.h"
#include "SplashScreen.h"
#include "NameSelectionWidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onCardSelected(int cardId);
    void onBuildClicked();
    void onDiscardClicked();
    void onWonderClicked();
    void onWonderChosen(int cardId);
    void onSplashFinished(SplashScreen::GameMode mode);
    void onNamesConfirmed(const QString& p1, const QString& p2, int difficulty = 1);
    void processAITurn();
    void onDiscardedCardsClicked();

private:
    Ui::MainWindow *ui;
    Game* m_game;
    SplashScreen::GameMode m_gameMode;

    int m_selectedCardId = -1;
    int m_draftPhase = 0;

    void renderGame();
    QString getColorHex(Color c);
    void updatePlayerInventories();
    void updateTurnIndicator();
    void startGame();
    void updateGameState();
    void startWonderDraft();

    // New Right Panel Layouts
    QVBoxLayout* m_p1ProgressLayout = nullptr;
    QVBoxLayout* m_p2ProgressLayout = nullptr;
    
    // Helper to get token description
    QString getTokenDescription(const QString& tokenName);
    
    // Slot for Info Button
    void onProgressInfoClicked();

    void rebuildRightPanel();
};
