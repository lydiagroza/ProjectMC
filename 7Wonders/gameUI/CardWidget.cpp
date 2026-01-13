#include "CardWidget.h"

CardWidget::CardWidget(int cardId, QWidget* parent)
    : QPushButton(parent), m_cardId(cardId), m_isFaceUp(false)
{
    // AICI SCHIMBĂM DIMENSIUNEA
    this->setFixedSize(70, 100); // Era 100, 150 -> Acum e mult mai compact
    this->setFlat(false);

    QFont font = this->font();
    font.setBold(true);
    font.setPointSize(7); // Micșorăm fontul să încapă pe cartea mică
    this->setFont(font);
}

void CardWidget::setupCard(const QString& name, const QString& colorCode, bool isFaceUp)
{
    m_isFaceUp = isFaceUp;

    if (m_isFaceUp) {
        // --- CARTE VIZIBILĂ ---
        this->setText(name);
        this->setEnabled(true);

        // Stil CSS pentru carte colorată
        QString style = QString(
            "QPushButton { "
            "  background-color: %1; "  // Culoarea din parametru
            "  color: white; "          // Text alb
            "  border: 2px solid #333; "
            "  border-radius: 6px; "
            "}"
            "QPushButton:hover { background-color: %1; filter: brightness(110%); }"
        ).arg(colorCode);

        this->setStyleSheet(style);

    }
    else {
        // --- CARTE ACOPERITĂ (SPATE) ---
        this->setText("Age Back");
        this->setEnabled(false); // Nu se poate da click

        // Stil gri generic
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
    // Când selectăm, schimbăm DOAR border-ul, păstrând culoarea de fundal
    // Nota: Aici e un mic hack vizual rapid.
    if (selected) {
        // Adaugă un border galben gros
        QString currentStyle = this->styleSheet();
        currentStyle.replace("border: 2px solid #333;", "border: 4px solid yellow;");
        this->setStyleSheet(currentStyle);
    }
    else {
        // Revino la border normal
        QString currentStyle = this->styleSheet();
        currentStyle.replace("border: 4px solid yellow;", "border: 2px solid #333;");
        this->setStyleSheet(currentStyle);
    }
}