#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <vector>
#include "Game.h" // Includem logica jocului (Backend)

// ... (include-uri existente)
#include <QMap>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    // ... sloturi existente ...
    void handleStartButton();
    void handleNextAgeButton();
    void onCardClicked();

private:
    Game m_game;

    // --- UI GENERAL ---
    QWidget* centralWidget;

    // --- UI PLAYER ZONES (NOU) ---
    QWidget* topPlayerZone;    // Inventarul Adversarului (Sus)
    QWidget* bottomPlayerZone; // Inventarul Tău (Jos)
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

    // FUNCȚII NOI PENTRU INVENTAR
    void setupPlayerZones(); // Creează containerele sus/jos
    void updatePlayerInventoryUI(Player* player, QWidget* zone); // Desenează cărțile micuțe

    void drawPyramid();
    void clearPyramidUI();
    void applyAgeStyle(int age);
};