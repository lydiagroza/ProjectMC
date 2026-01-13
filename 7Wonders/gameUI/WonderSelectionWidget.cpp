#include "WonderSelectionWidget.h"
#include <QVBoxLayout>

WonderSelectionWidget::WonderSelectionWidget(QWidget* parent)
    : QWidget(parent)
{
    // Layout vertical principal
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter); // Totul centrat, ca în poză

    // 1. Titlul (Ex: "Wonders selection - round 1 of 2")
    m_title = new QLabel("Wonders Selection", this);
    // CSS ca să arate ca o bandă de papirus sau cutie
    m_title->setStyleSheet(
        "QLabel { "
        "  background-color: #f39c12; " /* Portocaliu/Auriu */
        "  color: white; "
        "  font-size: 20px; font-weight: bold; "
        "  padding: 10px 40px; "
        "  border-radius: 10px; "
        "  border: 2px solid #d35400; "
        "}"
    );
    mainLayout->addWidget(m_title, 0, Qt::AlignCenter);
    mainLayout->addSpacing(30); // Spațiu între titlu și cărți

    // 2. Containerul pentru cărți (Grid 2x2)
    m_cardsContainer = new QWidget(this);
    m_layout = new QGridLayout(m_cardsContainer);
    m_layout->setSpacing(20); // Spațiu între cărți

    mainLayout->addWidget(m_cardsContainer, 0, Qt::AlignCenter);
}

void WonderSelectionWidget::setWonders(const std::vector<int>& ids, const std::vector<QString>& names, const std::vector<QString>& colors)
{
    // Ștergem ce era înainte
    qDeleteAll(m_currentCards);
    m_currentCards.clear();

    // Creăm cele 4 cărți
    for (int i = 0; i < ids.size(); ++i) {
        CardWidget* card = new CardWidget(ids[i], this);

        // --- TRUCUL VIZUAL: Facem cărțile astea MAI MARI ---
        card->setFixedSize(140, 200); // Dublu față de piramidă
        card->setupCard(names[i], colors[i], true);

        // Dacă aveai imagini, aici le puneai: 
        // card->setStyleSheet("border-image: url(Resources/sphinx.png);");

        connect(card, &QPushButton::clicked, this, &WonderSelectionWidget::onCardClicked);

        // Le așezăm în grilă: 0,0 | 0,1 | 1,0 | 1,1
        int row = i / 2;
        int col = i % 2;
        m_layout->addWidget(card, row, col);

        m_currentCards.append(card);
    }
}

void WonderSelectionWidget::onCardClicked()
{
    CardWidget* senderCard = qobject_cast<CardWidget*>(sender());
    if (senderCard) {
        emit wonderChosen(senderCard->getCardId());
    }
}