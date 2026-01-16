#include "DiscardedCardsDialog.h"
#include "ui_DiscardedCardsDialog.h"
#include "CardWidget.h"
#include "CardBase.h"
#include <QString>
#include <QStringList>

static QString getColorHexInternal(Color c)
{
    switch (c) {
    case Color::Blue:   return "#1565C0";
    case Color::Red:    return "#B71C1C";
    case Color::Green:  return "#2E7D32";
    case Color::Yellow: return "#F57F17";
    case Color::Brown:  return "#5D4037";
    case Color::Gray:   return "#616161";
    case Color::Purple: return "#6A1B9A";
    default: return "#8B7355";
    }
}

static QString formatCostInternal(const std::map<Resource, uint8_t>& costMap)
{
    QStringList parts;
    for (auto const& [res, count] : costMap) {
         QString icon;
         switch(res) {
             case Resource::Wood: icon = "wood.png"; break;
             case Resource::Clay: icon = "clay.png"; break;
             case Resource::Stone: icon = "stone.png"; break;
             case Resource::Glass: icon = "glass.png"; break;
             case Resource::Papyrus: icon = "papyrus.png"; break;
             case Resource::Coin: icon = "coin.png"; break;
         }
         if (!icon.isEmpty()) {
            parts << QString("%1 x <img src=':/resources/UI/%2' height='14'>").arg(count).arg(icon);
         }
    }
    return parts.join("<br>");
}

DiscardedCardsDialog::DiscardedCardsDialog(const std::vector<const CardBase*>& discardedCards, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DiscardedCardsDialog)
{
    ui->setupUi(this);
    setupCards(discardedCards);
}

DiscardedCardsDialog::~DiscardedCardsDialog()
{
    delete ui;
}

void DiscardedCardsDialog::setupCards(const std::vector<const CardBase*>& discardedCards)
{
    int row = 0;
    int col = 0;
    for (const auto* card : discardedCards)
    {
        if (!card) continue;
        
        CardWidget* cardWidget = new CardWidget(card->getId(), this);
        QString name = QString::fromStdString(card->getName());
        QString color = getColorHexInternal(card->getColor());
        QString cost = formatCostInternal(card->getCost());
        QString effect = QString::fromStdString(card->getEffectDescription());

        cardWidget->setupCard(name, color, true, cost, effect);
        ui->gridLayout->addWidget(cardWidget, row, col);

        col++;
        if (col == 5)
        {
            col = 0;
            row++;
        }
    }
}
