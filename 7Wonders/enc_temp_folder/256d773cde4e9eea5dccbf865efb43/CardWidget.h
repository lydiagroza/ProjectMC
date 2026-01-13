#pragma once
#include <QPushButton>

class CardWidget : public QPushButton
{
    Q_OBJECT

public:
    // Constructor
    CardWidget(int cardId, QWidget* parent = nullptr);

    // Funcția care colorează cartea
    void setupCard(const QString& name, const QString& colorCode, bool isFaceUp);

    // Funcția care pune border galben când dăm click
    void setSelected(bool selected);

    // Getter pentru a ști pe ce am dat click
    int getCardId() const { return m_cardId; }

private:
    int m_cardId;
    bool m_isFaceUp;
};