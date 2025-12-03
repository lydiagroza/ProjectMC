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
    void onCardClicked(); // Slot nou pentru când dai click pe o carte

private:
    Game m_game;

    // UI Elements
    QWidget* centralWidget;
    // Nu mai folosim Layout pentru carti, le punem cu coordonate fixe (setGeometry)
    // Dar pastram layout-ul doar pentru meniul de start
    QVBoxLayout* mainLayout;

    QPushButton* startButton;
    QLabel* infoLabel;

    // Lista butoanelor care reprezinta cartile (ca sa le putem sterge)
    std::vector<QPushButton*> m_cardButtons;

    // Functii ajutatoare
    void drawPyramid();
    void clearPyramidUI();
};