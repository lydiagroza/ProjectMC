#pragma once
#include <QPushButton>
#include "Game.h"

class CardWidget : public QPushButton
{
    Q_OBJECT

public:
    CardWidget(int cardId, QWidget* parent = nullptr);

    // Modificat: primește Numele și Culoarea (ex: "red", "green", "blue")
    void setupCard(const QString& name, const QString& colorCode, bool isFaceUp);

    // Rămâne la fel
    void setSelected(bool selected);
    int getCardId() const { return m_cardId; }

private:
    int m_cardId;
    bool m_isFaceUp;
};