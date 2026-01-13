#pragma once
#include <QPushButton>
#include <QGraphicsDropShadowEffect>  // ← ADAUGĂ

class CardWidget : public QPushButton
{
    Q_OBJECT

public:
    explicit CardWidget(int cardId, QWidget* parent = nullptr);
    void setupCard(const QString& name, const QString& colorCode, bool isFaceUp);
    void setSelected(bool selected);
    int getCardId() const { return m_cardId; }

private:
    int m_cardId;
    bool m_isFaceUp;
    QString adjustBrightness(const QString& color, double factor);
};