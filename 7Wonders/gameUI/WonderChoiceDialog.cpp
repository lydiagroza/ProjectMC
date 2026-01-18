#include "Player.h"
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
    int maxCols = 10; // Ensure horizontal layout

    for (auto* w : wonders) {
        CardWidget* cw = new CardWidget(w->getId(), this);
        cw->setFixedSize(300, 150); // Landscape display for Wonders

        int cost = player->findTotalCost(*w, *opponent);
        QString costStr;
        if (cost == GameConstants::IMPOSSIBLE_COST) {
            costStr = "Impossible";
            cw->setEnabled(false); // Grey out if too expensive
        } else {
            costStr = "Cost: " + QString::number(cost);
        }
        
        QString effect = QString::fromStdString(w->getEffectDescription());
        QString name = QString::fromStdString(w->getName());
        
        cw->setupCard(name, "#DAA520", true, costStr, effect); 

        //Check for wonder image
        QString imgPath = CardWidget::getWonderImagePath(w->getName().c_str());
        if (!imgPath.isEmpty()) {
            cw->setImage(imgPath);
        }
        
        connect(cw, &CardWidget::cardClicked, this, &WonderChoiceDialog::onCardClicked);

        ui->gridLayout->addWidget(cw, row, col);
        
        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
}

void WonderChoiceDialog::onCardClicked(int cardId)
{
    m_selectedWonderId = cardId;
    accept();
}