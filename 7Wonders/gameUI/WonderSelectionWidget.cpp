#include "WonderSelectionWidget.h"
#include "ui_WonderSelectionWidget.h"
#include "CardWidget.h" // Required for full definition of CardWidget
#include <QDebug> // For debugging

WonderSelectionWidget::WonderSelectionWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::WonderSelectionWidget)
{
    ui->setupUi(this);
}

WonderSelectionWidget::~WonderSelectionWidget()
{
    delete ui;
}

void WonderSelectionWidget::setWonders(const std::vector<int>& ids,
                                       const std::vector<QString>& names,
                                       const std::vector<QString>& colors,
                                       const std::vector<QString>& costs,
                                       const std::vector<QString>& effects)
{
    // --- Forceful Layout & Image Debugging ---

    // 1. Find and delete the old scroll area and all its children to ensure a clean slate.
    QScrollArea* oldScrollArea = this->findChild<QScrollArea*>();
    if (oldScrollArea) {
        delete oldScrollArea;
    }
    m_currentCards.clear();

    // 2. Create the scrollable area programmatically.
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // 3. Create a container and a horizontal layout for the cards.
    QWidget* cardsContainer = new QWidget();
    QHBoxLayout* cardsLayout = new QHBoxLayout(cardsContainer);
    cardsLayout->setSpacing(20);

    // 3. Create and add cards to the horizontal layout
    for (int i = 0; i < ids.size(); ++i) {
        qDebug() << "Processing wonder:" << names[i].trimmed().toLower();
        CardWidget* card = new CardWidget(ids[i], cardsContainer); // Parent to container
        
        card->setFixedSize(300, 150); // Landscape display for Wonders

        card->setupCard(names[i], colors[i], true, costs[i], effects[i]);

    // Check for wonder image
    QString imgPath = CardWidget::getWonderImagePath(names[i]);
    if (!imgPath.isEmpty()) {
        card->setImage(imgPath);
    }

        connect(card, &CardWidget::clicked, this, &WonderSelectionWidget::onCardClicked);
        cardsLayout->addWidget(card);
        m_currentCards.append(card);
    }
    
    // 5. Place the container in the scroll area and add it to the main layout.
    scrollArea->setWidget(cardsContainer);
    ui->verticalLayout->addWidget(scrollArea);
}

void WonderSelectionWidget::onCardClicked()
{
    CardWidget* senderCard = qobject_cast<CardWidget*>(sender());
    if (senderCard) {
        emit wonderChosen(senderCard->getCardId());
    }
}