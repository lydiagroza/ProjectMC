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
        // Larger size as requested
        wonderWidget->setFixedSize(180, 90); 

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
    
    // Add inventory cards
    // We want them to look like real cards, not blank.
    // We will use a separate horizontal layout or continue flow?
    // User wants "move the whole grid more to the left edge of the screen".
    // This implies the grid is maybe centered or has padding. 
    // The grid is inside 'ui->wondersLayout' which is inside a scroll area.
    // We should make sure alignment is Left.
    ui->wondersLayout->setAlignment(Qt::AlignLeft);
    
    // Create a container for inventory cards if needed, or just add them to the main layout
    // Since wonders are in a grid, let's put inventory in a HBox or Flow after the grid.
    // But 'ui->wondersLayout' is an HBox. We added 'gridContainer' to it.
    // We can add another container for inventory.
    
    QWidget* invContainer = new QWidget();
    QHBoxLayout* invLayout = new QHBoxLayout(invContainer);
    invLayout->setContentsMargins(10, 0, 0, 0); // Spacing from wonders
    invLayout->setSpacing(5);
    invLayout->setAlignment(Qt::AlignLeft);

    for (const auto& [color, cards] : inventory) {
        for (const auto& card : cards) {
            CardWidget* cardWidget = new CardWidget(card->getId(), this);
            cardWidget->setFixedSize(80, 110); // Keep inventory cards small but visible
            
            QString cName = QString::fromStdString(card->getName());
            QString cColorHex = ""; // We need to convert Color enum to hex string.
            // Helper locally or access global helper? 
            // We can replicate simple switch or pass empty and let CardWidget handle it if it knew enum.
            // CardWidget::setupCard takes a string color.
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
            
            // Pass cost and effect to keep the look!
            QString cCost = formatCost(card->getCost());
            QString cEffect = QString::fromStdString(card->getEffectDescription());

            cardWidget->setupCard(cName, cColorHex, true, cCost, cEffect);
            invLayout->addWidget(cardWidget);
        }
    }
    
    ui->wondersLayout->addWidget(invContainer);
    ui->wondersLayout->addStretch();
}