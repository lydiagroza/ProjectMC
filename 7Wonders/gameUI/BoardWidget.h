#pragma once
#include <QWidget>
#include <QList>
#include "CardWidget.h"

namespace Ui {
class BoardWidget;
}

class BoardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BoardWidget(QWidget* parent = nullptr);
    ~BoardWidget();

        void clearBoard();

        void placeCard(int id, QString name, QString color, bool isFaceUp, int row, int col, int totalCardsInRow, QString cost = "", QString effect = "");

    

    signals:

        void cardClicked(int cardId);

private slots:
    void handleInternalClick();

private:
    Ui::BoardWidget *ui;
    QList<CardWidget*> m_activeCards;

    // Constante geometrice
    const int CARD_WIDTH = 100;
    const int CARD_HEIGHT = 140;
    const int H_SPACING = 15; 
    const int V_OVERLAP = 50; 
};