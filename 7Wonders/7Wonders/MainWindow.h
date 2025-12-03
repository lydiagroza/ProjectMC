#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <vector>
#include "Game.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void handleStartButton();
    void onCardClicked();

private:
    Game m_game;

    // UI Elements
    QWidget* centralWidget;
    // Butoanele de meniu
    QPushButton* startButton;
    QLabel* titleLabel;     // Titlul mare (ex: "7 WONDERS DUEL")
    QLabel* ageLabel;       // Subtitlu (ex: "Age I")

    // Lista butoanelor (carti)
    std::vector<QPushButton*> m_cardButtons;

    // --- FUNCTII NOI ---
    void drawPyramid();
    void clearPyramidUI();
    void applyAgeStyle(int age); // Aici schimbam culorile!
};