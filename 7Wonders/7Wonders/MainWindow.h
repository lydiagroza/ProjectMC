#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include "Game.h"

class MainWindow : public QMainWindow {
    Q_OBJECT // Essential macro for Qt

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void handleStartButton();

private:
    Game m_game;

    // UI Elements (We are doing this without the designer for now to keep it simple)
    QWidget* centralWidget;
    QVBoxLayout* layout;
    QPushButton* startButton;
    QLabel* infoLabel;
};