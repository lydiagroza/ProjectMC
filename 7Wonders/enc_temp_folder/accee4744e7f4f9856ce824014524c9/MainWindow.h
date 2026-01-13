#pragma once
#include <QtWidgets/QMainWindow>
#include <QStackedWidget>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout> // <-- Add this include for QVBoxLayout

// Include-uri pentru clasele tale
#include "Game.h"
#include "BoardWidget.h"
#include "WonderSelectionWidget.h" // Asigură-te că ai creat această clasă (vezi răspunsul anterior)
#include "MilitaryTrackWidget.h"    // <-- Add this include

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    // Sloturi pentru interacțiunea cu jocul
    void onCardSelected(int cardId);      // Când dai click pe o carte din piramidă
    void onConstructClicked();            // Butonul din dreapta

    // Slot nou: Când jucătorul alege o minune din meniul de selecție
    void onWonderChosen(int cardId);

private:
    Game* m_game;            // Logica jocului (DLL)

    // Elementele de interfață
    QStackedWidget* m_stack;             // Teancul de ecrane (Mijloc)
    BoardWidget* m_boardWidget;          // Ecranul 1: Piramida
    WonderSelectionWidget* m_wonderSelection; // Ecranul 0: Selecția

    MilitaryTrackWidget* m_militaryTrackWidget; // Add this for the military track

    QFrame* m_opponentZone;              // <-- ADD THIS LINE
    QFrame* m_playerZone;                // (optional: for symmetry, if used elsewhere)
    QLabel* m_opponentLabel;             // <-- ADD THIS LINE
    QLabel* m_playerLabel;               // <-- ADD THIS LINE

    QVBoxLayout* m_progressTokensLayout; // Add this layout to hold progress tokens

    int m_selectedCardId = -1;           // ID-ul cărții selectate curent
    int m_draftPhase = 0;                // Adaugă această linie pentru a declara m_draftPhase

    // Funcții ajutătoare
    void renderGame();                   // Desenează piramida
    void setupDummyWonders();            // Funcție de test pentru ecranul de minuni
    QString getColorHex(Color c);        // Conversie culori
    void updatePlayerInventories();
    void startGame();                    // Adaugă această declarație pentru a se potrivi cu implementarea din MainWindow.cpp
    void updateGameState();
    void startWonderDraft();
};