#pragma once
#include <QtWidgets/QMainWindow>
#include <QStackedWidget>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout> 
#include <QMediaPlayer>
#include <QAudioOutput>
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
    void onSaveGameClicked();

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

    // New layouts for the Right Panel
    QVBoxLayout* m_p1ProgressLayout = nullptr;
    QVBoxLayout* m_p2ProgressLayout = nullptr;
    
    // Helper function for token descriptions
    QString getTokenDescription(const QString& tokenName);
    
    // Slot for the Info Button
    void onProgressInfoClicked();
    void onDuckHintClicked();

    void rebuildRightPanel();

    // Labels for the Progress Section
    QLabel* m_lblProgressP1 = nullptr;
    QLabel* m_lblProgressP2 = nullptr;
    QPushButton* m_duckBtn = nullptr;

    QMediaPlayer* m_player = nullptr;
    QAudioOutput* m_audioOutput = nullptr;

    QMediaPlayer* m_musicPlayer = nullptr;
    QAudioOutput* m_musicOutput = nullptr;
};
