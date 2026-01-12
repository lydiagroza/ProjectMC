#include "CardWidget.h"

CardWidget::CardWidget(int cardId, QWidget* parent)
    : QPushButton(parent), m_cardId(cardId)
{
    // Setări vizuale de bază (opțional)
    this->setFixedSize(100, 150); // Dimensiunea unei cărți în pixeli
    this->setStyleSheet("border: 1px solid black;"); // Un contur simplu
}

void CardWidget::setCardImage(const QString& imagePath)
{
    // Aici încărcăm imaginea pe buton
    QPixmap pixmap(imagePath);
    QIcon buttonIcon(pixmap);
    this->setIcon(buttonIcon);
    this->setIconSize(this->size());
}