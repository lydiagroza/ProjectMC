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

            void placeCard(int id, QString name, QString color, bool isFaceUp, int row, int col, int totalCardsInRow, QString cost = "", QString effect = "", QString unlocks = "");

    

    signals:

        void cardClicked(int cardId);

private slots:
    void handleInternalClick(int cardId);

public:
    void setScalingFactor(double scale) { m_scaleFactor = scale; }

private:
    Ui::BoardWidget *ui;
    QList<CardWidget*> m_activeCards;
    double m_scaleFactor = 1.0;

    // Constante geometrice (Base size)
    const int BASE_CARD_WIDTH = 100;
    const int BASE_CARD_HEIGHT = 140;
    const int BASE_H_SPACING = 15; 
    const int BASE_V_OVERLAP = 80; 
};