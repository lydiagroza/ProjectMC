#include "WonderSelectionWidget.h"
#include "ui_WonderSelectionWidget.h"

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

void WonderSelectionWidget::setWonders(const std::vector<int>& ids, const std::vector<QString>& names, const std::vector<QString>& colors)
{
    qDeleteAll(m_currentCards);
    m_currentCards.clear();

    for (int i = 0; i < ids.size(); ++i) {
        CardWidget* card = new CardWidget(ids[i], this);

        // Make cards larger for selection screen
        card->setFixedSize(140, 200);
        card->setupCard(names[i], colors[i], true);

        connect(card, &CardWidget::clicked, this, &WonderSelectionWidget::onCardClicked);

        int row = i / 2;
        int col = i % 2;
        
        // Add to the grid layout defined in the .ui file
        ui->gridLayout->addWidget(card, row, col);

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