#include "../../GameBase/Player.h"
#include "WonderChoiceDialog.h"
#include "ui_WonderChoiceDialog.h"
#include "CardWidget.h"
#include "GameConstants.h"

WonderChoiceDialog::WonderChoiceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WonderChoiceDialog)
{
    ui->setupUi(this);
}

WonderChoiceDialog::~WonderChoiceDialog()
{
    delete ui;
}

void WonderChoiceDialog::setWonders(const std::vector<Wonder*>& wonders, Player* player, Player* opponent)
{
    // Clear layout
    QLayoutItem* item;
    while ((item = ui->gridLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    int row = 0;
    int col = 0;
    int maxCols = 3;

    for (auto* w : wonders) {
        CardWidget* cw = new CardWidget(w->getId(), this);
        cw->setFixedSize(140, 200); // Large display

        int cost = player->findTotalCost(*w, *opponent);
        QString costStr;
        if (cost == GameConstants::IMPOSSIBLE_COST) {
            costStr = "Impossible";
            cw->setEnabled(false); // Grey out if too expensive
        } else {
            costStr = "Cost: " + QString::number(cost);
        }

        // Format raw cost for display logic if needed, but here we just show total coins needed
        // To be nicer, we could show the resource cost. 
        // For now, let's just use the name and effect.
        
        QString effect = QString::fromStdString(w->getEffectDescription());
        QString name = QString::fromStdString(w->getName());
        
        cw->setupCard(name, "#DAA520", true, costStr, effect); 
        
        connect(cw, &CardWidget::clicked, this, &WonderChoiceDialog::onCardClicked);

        ui->gridLayout->addWidget(cw, row, col);
        
        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
}

void WonderChoiceDialog::onCardClicked()
{
    CardWidget* cw = qobject_cast<CardWidget*>(sender());
    if (cw) {
        m_selectedWonderId = cw->getCardId();
        accept(); // Close dialog with Accepted result
    }
}