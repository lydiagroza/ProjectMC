#include "CardWidget.h"

CardWidget::CardWidget(int cardId, QWidget* parent)
    : QPushButton(parent), m_cardId(cardId), m_isFaceUp(false)
{
    this->setFixedSize(100, 150); // Dimensiunea unei cărți
    this->setFlat(false);

    // Font un pic mai mare și bold ca să se vadă numele
    QFont font = this->font();
    font.setBold(true);
    font.setPointSize(10);
    this->setFont(font);
}

void CardWidget::setupCard(const QString& name, const QString& colorCode, bool isFaceUp)
{
    m_isFaceUp = isFaceUp;

    if (m_isFaceUp) {
        // --- CARTE VIZIBILĂ ---
        this->setText(name); // Scriem numele (ex: "Baths")
        this->setEnabled(true);

        // Setăm culoarea de fundal în funcție de tipul cărții
        // colorCode va fi ceva gen: "#e74c3c" (Roșu) sau "blue", "green"
        QString style = QString(
            "QPushButton { "
            "  background-color: %1; "
            "  color: white; "        // Scris alb
            "  border: 2px solid #333; "
            "  border-radius: 5px; "
            "}"
            "QPushButton:hover { filter: brightness(110%); }" // Efect la mouse over
        ).arg(colorCode);

        this->setStyleSheet(style);

    }
    else {
        // --- CARTE ACOPERITĂ (SPATE) ---
        this->setText("???"); // Nu știm ce e
        this->setEnabled(false); // Nu putem da click

        // O culoare gri/maro generică pentru spate
        this->setStyleSheet(
            "QPushButton { "
            "  background-color: #95a5a6; " // Un gri neutru
            "  color: #2c3e50; "
            "  border: 2px dashed #7f8c8d; "
            "  border-radius: 5px; "
            "}"
        );
    }
}

void CardWidget::setSelected(bool selected)
{
    // Când selectăm, adăugăm un contur GALBEN gros peste stilul existent
    // Notă: Aici e un mic truc, în mod real am re-aplica stilul complet, 
    // dar pentru moment putem schimba doar border-ul dacă reținem culoarea.

    // Varianta simplă: Dacă e selectat, îi schimbăm textul să arate asta vizual
    if (selected) {
        QString currentStyle = this->styleSheet();
        // Adăugăm border galben la stilul curent
        this->setStyleSheet(currentStyle + "border: 4px solid yellow;");
    }
    else {
        // Resetăm borderul (trebuie reapelat setupCard ideal, dar merge și așa pt test)
        // Cel mai corect e să reții culoarea în clasă și să o reaplici.
    }
}