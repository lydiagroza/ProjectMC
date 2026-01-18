#include "WonderSelectionWidget.h"
#include "ui_WonderSelectionWidget.h"
#include "CardWidget.h" 
#include <QDebug> 

#include <QGridLayout>

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
    //Clear existing layout content
    QLayoutItem* item;
    while ((item = ui->cardsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    m_currentCards.clear();

    QGridLayout* gridLayout = qobject_cast<QGridLayout*>(ui->cardsLayout);
    if (!gridLayout) {
        qWarning("The layout 'cardsLayout' is not a QGridLayout.");
        return;
    }

    for (int i = 0; i < (int)ids.size(); ++i) {
        CardWidget* card = new CardWidget(ids[i], ui->scrollAreaWidgetContents); 
        
        card->setFixedSize(300, 150);

        card->setupCard(names[i], colors[i], true, costs[i], effects[i]);

        QString imgPath = CardWidget::getWonderImagePath(names[i]);
        if (!imgPath.isEmpty()) {
            card->setImage(imgPath);
        }

        connect(card, &CardWidget::cardClicked, this, &WonderSelectionWidget::onCardClicked);

        int row = i / 2;
        int col = i % 2;
        gridLayout->addWidget(card, row, col);
        m_currentCards.append(card);
    }
}

void WonderSelectionWidget::onCardClicked(int cardId)
{
    emit wonderChosen(cardId);
}