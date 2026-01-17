#include "WonderSelectionWidget.h"
#include "ui_WonderSelectionWidget.h"
#include "CardWidget.h" 
#include <QDebug> 

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
    // Clear existing layout content (placeholder or old cards)
    QLayoutItem* item;
    while ((item = ui->cardsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    m_currentCards.clear();

    // Re-populate using the existing layout from UI
    for (int i = 0; i < (int)ids.size(); ++i) {
        // Use the scroll area's content widget as parent
        CardWidget* card = new CardWidget(ids[i], ui->scrollAreaWidgetContents); 
        
        card->setFixedSize(300, 150); // Landscape display for Wonders

        card->setupCard(names[i], colors[i], true, costs[i], effects[i]);

        // Check for wonder image
        QString imgPath = CardWidget::getWonderImagePath(names[i]);
        if (!imgPath.isEmpty()) {
            card->setImage(imgPath);
        }

        connect(card, &CardWidget::cardClicked, this, &WonderSelectionWidget::onCardClicked);
        ui->cardsLayout->addWidget(card);
        m_currentCards.append(card);
    }
    
    // Add stretch to push cards to the left if few
    ui->cardsLayout->addStretch();
}

void WonderSelectionWidget::onCardClicked(int cardId)
{
    emit wonderChosen(cardId);
}