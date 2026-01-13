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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onCardSelected(int cardId);      
    void onConstructClicked();          

    void onWonderChosen(int cardId);

private:
    Game* m_game;           


    QStackedWidget* m_stack;            
    BoardWidget* m_boardWidget;         
    WonderSelectionWidget* m_wonderSelection;

    MilitaryTrackWidget* m_militaryTrackWidget; 
    QFrame* m_opponentZone;            
    QFrame* m_playerZone;               
    QLabel* m_opponentLabel;          
    QLabel* m_playerLabel;              

    QVBoxLayout* m_progressTokensLayout; 

    int m_selectedCardId = -1;          
    int m_draftPhase = 0;                

    // Funcții ajutătoare
    void renderGame();                          
    QString getColorHex(Color c);        
    void updatePlayerInventories();
    void startGame();                    /
    void updateGameState();
    void startWonderDraft();
};