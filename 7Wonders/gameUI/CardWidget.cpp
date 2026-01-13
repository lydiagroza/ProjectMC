#include "CardWidget.h"

CardWidget::CardWidget(int cardId, QWidget* parent)
    : QPushButton(parent), m_cardId(cardId), m_isFaceUp(false)
{
    this->setFixedSize(70, 100);
    this->setFlat(false);

    QFont font = this->font();
    font.setBold(true);
    font.setPointSize(7);
    this->setFont(font);
}

void CardWidget::setupCard(const QString& name, const QString& colorCode, bool isFaceUp)
{
    m_isFaceUp = isFaceUp;

    if (m_isFaceUp) {
        // --- CARTE VIZIBILĂ ---
        this->setText(name);
        this->setEnabled(true);

        // ❌ PROBLEMA ERA AICI: "filter: brightness(110%)" NU EXISTĂ ÎN QT!
        // ✅ SOLUȚIE: Calculează manual o culoare mai deschisă pentru hover
        QString style = QString(
            "QPushButton { "
            "  background-color: %1; "
            "  color: white; "
            "  border: 2px solid #333; "
            "  border-radius: 6px; "
            "}"
            "QPushButton:hover { "
            "  background-color: %1; "
            "  border: 2px solid #f39c12; " // Border galben la hover
            "}"
        ).arg(colorCode);

        this->setStyleSheet(style);

    }
    else {
        // --- CARTE ACOPERITĂ (SPATE) ---
        this->setText("?");
        this->setEnabled(false);

        this->setStyleSheet(
            "QPushButton { "
            "  background-color: #7f8c8d; "
            "  color: #bdc3c7; "
            "  border: 2px dashed #95a5a6; "
            "  border-radius: 6px; "
            "}"
        );
    }
}

void CardWidget::setSelected(bool selected)
{
    if (!m_isFaceUp) return; // Nu selecta cărți cu fața în jos

    if (selected) {
        // Border galben gros pentru selecție
        QString currentStyle = this->styleSheet();
        currentStyle.replace("border: 2px solid #333;", "border: 4px solid #f1c40f;");
        this->setStyleSheet(currentStyle);
    }
    else {
        // Revino la border normal
        QString currentStyle = this->styleSheet();
        currentStyle.replace("border: 4px solid #f1c40f;", "border: 2px solid #333;");
        this->setStyleSheet(currentStyle);
    }
}