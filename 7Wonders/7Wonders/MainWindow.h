#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <vector>
#include "Game.h"
#include <QMap>

class MainWindow : public QMainWindow {
   

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void handleStartButton();
    void handleNextAgeButton();
    void onCardClicked();

private:
    Game m_game;

    // --- UI GENERAL ---
    QWidget* centralWidget;

    // ELEMENTELE CARE LIPSEAU:
    QLabel* titleLabel;         // <--- ADAUGAT
    QLabel* ageLabel;           // <--- ADAUGAT
    QPushButton* startButton;   // <--- ADAUGAT
    QPushButton* nextAgeButton; // <--- ADAUGAT

    // --- UI PLAYER ZONES ---
    QWidget* topPlayerZone;
    QWidget* bottomPlayerZone;
    QLabel* topPlayerLabel;
    QLabel* bottomPlayerLabel;

    // --- UI PIRAMIDA ---
    std::vector<QPushButton*> m_cardButtons;

    // --- UI DREAPTA ---
    QWidget* rightPanel;
    QPushButton* militaryPawn;
    std::vector<QPushButton*> progressTokens;

    // --- FUNCȚII ---
    void setupRightPanel();
    void updateMilitaryPawn();

    void setupPlayerZones();
    void updatePlayerInventoryUI(Player* player, QWidget* zone);

    void drawPyramid();
    void clearPyramidUI();
    void applyAgeStyle(int age);
};