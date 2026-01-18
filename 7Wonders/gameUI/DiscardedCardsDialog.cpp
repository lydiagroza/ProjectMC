#include "DiscardedCardsDialog.h"
#include "ui_DiscardedCardsDialog.h"
#include "CardBase.h"
#include "CardWidget.h"
#include <QString>
#include <QStringList>

static QString getColorHex(Color c)
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

static QString getSymbolEmoji(const QString& symbolName) {
    QString lowerSymbolName = symbolName.toLower();
    if (lowerSymbolName == "moon") return "🌙";
    if (lowerSymbolName == "sun") return "☀️";
    if (lowerSymbolName == "mask") return "🎭";
    if (lowerSymbolName == "column") return "🏛️";
    if (lowerSymbolName == "droplet") return "💧";
    if (lowerSymbolName == "temple") return "⛩️";
    if (lowerSymbolName == "book") return "📖";
    if (lowerSymbolName == "gear") return "⚙️";
    if (lowerSymbolName == "lyre") return "🪕";
    if (lowerSymbolName == "pot") return "🏺";
    if (lowerSymbolName == "horse") return "🐎";
    if (lowerSymbolName == "sword") return "⚔️";
    if (lowerSymbolName == "castle") return "🏰";
    if (lowerSymbolName == "target") return "🎯";
    if (lowerSymbolName == "helmet") return "🪖";
    if (lowerSymbolName == "vase") return "🏺";
    if (lowerSymbolName == "barrel") return "🛢️";
    return "";
}

static QString formatCost(const std::map<Resource, uint8_t>& costMap, std::optional<Symbol> symbol)
{
    QStringList parts;
    if (symbol.has_value()) {
        parts << getSymbolEmoji(QString::fromStdString(to_string(symbol.value())));
    }
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
};

DiscardedCardsDialog::DiscardedCardsDialog(const std::vector<const CardBase*>& discardedCards, QWidget *parent) :
    QDialog(parent),
    ui(new Ui_DiscardedCardsDialog),
    m_selectedCardId(-1)
{
    ui->setupUi(this);
    setupCards(discardedCards);
}

DiscardedCardsDialog::~DiscardedCardsDialog()
{
    delete ui;
}

int DiscardedCardsDialog::getSelectedCardId() const
{
    return m_selectedCardId;
}

void DiscardedCardsDialog::onCardClicked(int cardId)
{
    m_selectedCardId = cardId;
    emit cardSelected(cardId);
    accept();
}

void DiscardedCardsDialog::setupCards(const std::vector<const CardBase*>& discardedCards)
{
    int row = 0;
    int col = 0;
    for (const auto* card : discardedCards)
    {
        CardWidget* cardWidget = new CardWidget(card->getId(), this);
        QString name = QString::fromStdString(card->getName());
        QString color = getColorHex(card->getColor());
        QString cost = formatCost(card->getCost(), card->getSymbol());
        QString effect = QString::fromStdString(card->getEffectDescription());
        QString unlocks_string;
        if(card->getUnlocks().has_value()){
            unlocks_string = QString::fromStdString(to_string(card->getUnlocks().value()));
        }

        cardWidget->setupCard(name, color, true, cost, effect, unlocks_string);
        connect(cardWidget, &CardWidget::cardClicked, this, &DiscardedCardsDialog::onCardClicked);
        ui->gridLayout->addWidget(cardWidget, row, col);

        col++;
        if (col == 5)
        {
            col = 0;
            row++;
        }
    }
}