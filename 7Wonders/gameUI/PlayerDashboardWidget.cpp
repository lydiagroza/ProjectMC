#include "PlayerDashboardWidget.h"
#include "ui_PlayerDashboardWidget.h"
#include "CardWidget.h"
#include <QGraphicsDropShadowEffect>

PlayerDashboardWidget::PlayerDashboardWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PlayerDashboardWidget)
{
    ui->setupUi(this);

    // Initial default style (generic)
    this->setStyleSheet(
        "QWidget#PlayerDashboardWidget { "
        "  border: 3px solid #8B4513; "
        "  border-radius: 10px; "
        "  background: #2C1810; "
        "}"
    );

    // Shadow for text
    QGraphicsDropShadowEffect* textShadow = new QGraphicsDropShadowEffect();
    textShadow->setBlurRadius(8);
    textShadow->setColor(QColor(0, 0, 0, 180));
    textShadow->setOffset(2, 2);
    ui->infoLabel->setGraphicsEffect(textShadow);
}

PlayerDashboardWidget::~PlayerDashboardWidget()
{
    delete ui;
}

void PlayerDashboardWidget::setTheme(bool isOpponent)
{
    if (isOpponent) {
        this->setStyleSheet(
            "QWidget#PlayerDashboardWidget { "
            "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
            "    stop:0 #8B0000, stop:1 #5C0000); "
            "  border: 3px solid #8B4513; "
            "  border-radius: 10px; "
            "}"
        );
        ui->infoLabel->setText("⚔️ OPPONENT DUCK EMPIRE ⚔️");
    } else {
        this->setStyleSheet(
            "QWidget#PlayerDashboardWidget { "
            "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
            "    stop:0 #1565C0, stop:1 #0D47A1); "
            "  border: 3px solid #8B4513; "
            "  border-radius: 10px; "
            "}"
        );
        ui->infoLabel->setText("🏛️ YOUR DUCK CIVILIZATION 🏛️");
    }
}

void PlayerDashboardWidget::updateDashboard(const std::string& name, int coins, 
                                         const std::map<Resource, std::uint8_t>& resources,
                                         const std::vector<std::shared_ptr<Wonder>>& wonders,
                                         const std::map<Color, std::vector<std::shared_ptr<CardBase>>>& inventory,
                                         int victoryPoints)
{
    // Update Text
    QString icon = (this->styleSheet().contains("#8B0000")) ? "⚔️" : "🏛️";
    
    auto getResCount = [&](Resource r) {
        return resources.count(r) ? resources.at(r) : 0;
    };

    QString resString = QString(
        "<img src=':/resources/UI/wood.png' height='16'> %1 "
        "<img src=':/resources/UI/clay.png' height='16'> %2 "
        "<img src=':/resources/UI/stone.png' height='16'> %3 "
        "<img src=':/resources/UI/glass.png' height='16'> %4 "
        "<img src=':/resources/UI/papyrus.png' height='16'> %5"
    ).arg(getResCount(Resource::Wood))
     .arg(getResCount(Resource::Clay))
     .arg(getResCount(Resource::Stone))
     .arg(getResCount(Resource::Glass))
     .arg(getResCount(Resource::Papyrus));

    QString text = QString(" %1 <b>%2</b> | 🏆 VP: <b>%5</b> | <img src=':/resources/UI/coin.png' height='16'> <b>%3</b> | %4")
        .arg(icon)
        .arg(QString::fromStdString(name))
        .arg(coins)
        .arg(resString)
        .arg(victoryPoints);
    
    ui->infoLabel->setText(text);

    // --- REFACTOR: SPLIT DASHBOARD ---
    
    // Clear the main container layout
    QLayoutItem* item;
    while ((item = ui->wondersLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    // 1. Left Zone: Wonders (Fixed Grid)
    QFrame* wonderZone = new QFrame();
    wonderZone->setFixedWidth(420); // Wide enough for 2 columns of 200px cards
    wonderZone->setStyleSheet("background: rgba(0,0,0,0.2); border-radius: 5px;");
    
    QVBoxLayout* wonderZoneLayout = new QVBoxLayout(wonderZone);
    QLabel* wonderLabel = new QLabel("✨ WONDERS");
    wonderLabel->setStyleSheet("color: #DAA520; font-weight: bold; font-size: 10px; margin-bottom: 2px;");
    wonderLabel->setAlignment(Qt::AlignCenter);
    wonderZoneLayout->addWidget(wonderLabel);

    QGridLayout* wonderGrid = new QGridLayout();
    wonderGrid->setSpacing(5);
    wonderGrid->setContentsMargins(5,5,5,5);
    wonderZoneLayout->addLayout(wonderGrid);
    wonderZoneLayout->addStretch(); // Push to top

    // Populate Wonders
        auto getSymbolEmoji = [](const QString& symbolName) -> QString {
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
        };
        
        auto formatCost = [&](const std::map<Resource, uint8_t>& costMap, std::optional<Symbol> symbol) -> QString {
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
            return parts.join(" ");
        };
    
        int wIdx = 0;
        for (const auto& wonderPtr : wonders) {
            CardWidget* wonderWidget = new CardWidget(wonderPtr->getId(), wonderZone);
            wonderWidget->setFixedSize(200, 100); // Larger Size
    
            QString wName = QString::fromStdString(wonderPtr->getName());
            QString color = wonderPtr->getIsBuilt() ? "#DAA520" : "#6D4C41";
            QString effect = QString::fromStdString(wonderPtr->getEffectDescription());
            QString cost = formatCost(wonderPtr->getCost(), wonderPtr->getSymbol());
    
            wonderWidget->setupCard(wName, color, true, cost, effect);
            QString imgPath = CardWidget::getWonderImagePath(wName);
            if (!imgPath.isEmpty()) wonderWidget->setImage(imgPath);
    
            wonderGrid->addWidget(wonderWidget, wIdx / 2, wIdx % 2);
            wIdx++;
        }
    
        // 2. Separator
        QFrame* vLine = new QFrame();
        vLine->setFrameShape(QFrame::VLine);
        vLine->setFrameShadow(QFrame::Sunken);
        vLine->setStyleSheet("color: #8B4513; background: #8B4513; width: 2px;");
    
        // 3. Right Zone: Inventory (Scrollable Row)
        QFrame* inventoryZone = new QFrame();
        QVBoxLayout* invZoneLayout = new QVBoxLayout(inventoryZone);
        invZoneLayout->setContentsMargins(0,0,0,0);
        
        QLabel* invLabel = new QLabel("🏛️ CIVILIZATION");
        invLabel->setStyleSheet("color: #A5D6A7; font-weight: bold; font-size: 10px; margin-bottom: 2px;");
        invLabel->setAlignment(Qt::AlignCenter);
        invZoneLayout->addWidget(invLabel);
    
        QScrollArea* invScroll = new QScrollArea();
        invScroll->setWidgetResizable(true);
        invScroll->setStyleSheet("background: transparent; border: none;");
        invScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        invScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        
        QWidget* invContent = new QWidget();
        QHBoxLayout* invLayout = new QHBoxLayout(invContent);
        invLayout->setSpacing(5);
        invLayout->setContentsMargins(5,5,5,5);
        invLayout->setAlignment(Qt::AlignLeft);
    
        // Populate Inventory
        for (const auto& [color, cards] : inventory) {
            for (const auto& card : cards) {
                CardWidget* cardWidget = new CardWidget(card->getId(), invContent);
                cardWidget->setFixedSize(80, 110); 
                
                QString cName = QString::fromStdString(card->getName());
                QString cColorHex;
                switch(color) {
                    case Color::Blue:   cColorHex = "#1565C0"; break;
                    case Color::Red:    cColorHex = "#B71C1C"; break;
                    case Color::Green:  cColorHex = "#2E7D32"; break;
                    case Color::Yellow: cColorHex = "#F57F17"; break;
                    case Color::Brown:  cColorHex = "#5D4037"; break;
                    case Color::Gray:   cColorHex = "#616161"; break;
                    case Color::Purple: cColorHex = "#6A1B9A"; break;
                    default: cColorHex = "#8B7355"; break;
                }
                
                QString cCost = formatCost(card->getCost(), card->getSymbol());
                QString cEffect = QString::fromStdString(card->getEffectDescription());
                QString cUnlocks;
                if(card->getUnlocks().has_value()){
                    cUnlocks = QString::fromStdString(to_string(card->getUnlocks().value()));
                }
    
                cardWidget->setupCard(cName, cColorHex, true, cCost, cEffect, cUnlocks);
                invLayout->addWidget(cardWidget);
            }
        }    invLayout->addStretch();
    
    invScroll->setWidget(invContent);
    invZoneLayout->addWidget(invScroll);

    // Add everything to Main Layout
    ui->wondersLayout->addWidget(wonderZone);
    ui->wondersLayout->addWidget(vLine);
    ui->wondersLayout->addWidget(inventoryZone, 1); // Expand to fill rest
}