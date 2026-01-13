#pragma once
#include <QWidget>
#include <QList>
#include "CardWidget.h"

class BoardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BoardWidget(QWidget* parent = nullptr);

    // Șterge tot (între epoci sau la refresh)
    void clearBoard();

    // Adaugă o carte la coordonatele specificate
    void placeCard(int id, QString name, QString color, bool isFaceUp, int row, int col, int totalCardsInRow);

signals:
    // Semnal trimis către MainWindow
    void cardClicked(int cardId);

private slots:
    void handleInternalClick();

private:
    QList<CardWidget*> m_activeCards;

    // Constante geometrice
    const int CARD_WIDTH = 70;
    const int CARD_HEIGHT = 100;
    const int H_SPACING = 10; // Spațiu orizontal
    const int V_OVERLAP = 25; // Suprapunere verticală
};