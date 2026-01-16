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

    // Update Wonders and Inventory
    // Wonders are now in a GRID for compact view
    // Clean up existing grid items if it was already converted, but since ui->wondersLayout is defined as HBox in UI,
    // we should have changed it or we need to repurpose the container.
    // However, we cannot change the class of ui->wondersLayout easily if it's from UI.
    // Use deleteLayout trick or just add a grid layout inside the existing layout.
    
    QLayoutItem* item;
    while ((item = ui->wondersLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    
    // Create a Grid Layout container if not already there, OR just handle the HBox.
    // Since the request asks for 2x2 grid, and we have an HBox, we can add a widget that HAS a grid layout.
    
    QWidget* gridContainer = new QWidget();
    QGridLayout* grid = new QGridLayout(gridContainer);
    grid->setContentsMargins(0,0,0,0);
    grid->setSpacing(2);
    
    ui->wondersLayout->addWidget(gridContainer);

    auto formatCost = [](const std::map<Resource, uint8_t>& costMap) -> QString {
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
        return parts.join(" ");
    };

    int wIdx = 0;
    for (const auto& wonderPtr : wonders) {
        CardWidget* wonderWidget = new CardWidget(wonderPtr->getId(), ui->wondersContainer);
        // Smaller size for compact grid
        wonderWidget->setFixedSize(140, 70); 

        QString wName = QString::fromStdString(wonderPtr->getName());
        QString color = wonderPtr->getIsBuilt() ? "#DAA520" : "#6D4C41";
        QString effect = QString::fromStdString(wonderPtr->getEffectDescription());
        QString cost = formatCost(wonderPtr->getCost());

        wonderWidget->setupCard(wName, color, true, cost, effect);

        QString imgPath = CardWidget::getWonderImagePath(wName);
        if (!imgPath.isEmpty()) {
            wonderWidget->setImage(imgPath);
        }

        // 2 columns
        grid->addWidget(wonderWidget, wIdx / 2, wIdx % 2);
        wIdx++;
    }
    
    // Add inventory cards after (or separately?)
    // Inventory cards (Blue/Green/etc) might clutter the wonder grid. 
    // They usually go to the side. 
    // We'll add them to the main layout after the grid container.
    
    for (const auto& [color, cards] : inventory) {
        for (const auto& card : cards) {
            CardWidget* cardWidget = new CardWidget(card->getId(), this);
            cardWidget->setFixedSize(80, 110); // Smaller inventory cards
            cardWidget->setupCard(QString::fromStdString(card->getName()), "", true, "", "");
            ui->wondersLayout->addWidget(cardWidget);
        }
    }

    ui->wondersLayout->addStretch();
}