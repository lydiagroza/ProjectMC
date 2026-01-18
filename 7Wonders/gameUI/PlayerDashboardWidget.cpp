#include "PlayerDashboardWidget.h"
#include "ui_PlayerDashboardWidget.h"
#include "CardWidget.h"
#include <QGraphicsDropShadowEffect>
#include <QPushButton>
#include <QMessageBox>
#include <QScrollArea>

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
                                         int victoryPoints,
                                         const std::vector<std::shared_ptr<ProgressToken>>& tokens)
{
    // Store data for popup
    m_activeEffectsWonders = wonders;
    m_activeEffectsTokens = tokens;

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

    QLayoutItem* item;
    while ((item = ui->wondersGrid->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

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
        CardWidget* wonderWidget = new CardWidget(wonderPtr->getId(), ui->wondersZone);
        wonderWidget->setFixedSize(200, 100); 

        QString wName = QString::fromStdString(wonderPtr->getName());
        QString color = wonderPtr->getIsBuilt() ? "#DAA520" : "#6D4C41";
        QString effect = QString::fromStdString(wonderPtr->getEffectDescription());
        QString cost = formatCost(wonderPtr->getCost());

        wonderWidget->setupCard(wName, color, true, cost, effect);
        QString imgPath = CardWidget::getWonderImagePath(wName);
        if (!imgPath.isEmpty()) wonderWidget->setImage(imgPath);

        ui->wondersGrid->addWidget(wonderWidget, wIdx / 2, wIdx % 2);
        wIdx++;
    }

    while ((item = ui->invLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    QWidget* invContainer = ui->scrollAreaWidgetContents;

    for (const auto& [color, cards] : inventory) {
        for (const auto& card : cards) {
            CardWidget* cardWidget = new CardWidget(card->getId(), invContainer);
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
            
            QString cCost = formatCost(card->getCost());
            QString cEffect = QString::fromStdString(card->getEffectDescription());

            cardWidget->setupCard(cName, cColorHex, true, cCost, cEffect);
            ui->invLayout->addWidget(cardWidget);
        }
    }
    
    m_effectsBtn = new QPushButton("🔮 Active\nEffects");
    m_effectsBtn->setFixedSize(80, 110); 
    m_effectsBtn->setCursor(Qt::PointingHandCursor);
    m_effectsBtn->setStyleSheet(
        "QPushButton { "
        "  background: qradialgradient(cx:0.5, cy:0.5, radius: 0.5, fx:0.5, fy:0.5, stop:0 #7B1FA2, stop:1 #4A148C); "
        "  color: #E1BEE7; "
        "  border: 2px solid #8E24AA; "
        "  border-radius: 8px; "
        "  font-weight: bold; "
        "  font-size: 12px; "
        "}"
        "QPushButton:hover { "
        "  background: #8E24AA; "
        "  color: white; "
        "}"
    );
    connect(m_effectsBtn, &QPushButton::clicked, this, &PlayerDashboardWidget::onEffectsClicked);
    
    ui->invLayout->addWidget(m_effectsBtn);
    ui->invLayout->addStretch();
}

// Simple helper function for token descriptions (Duplicated from MainWindow, should be static/common)
static QString getLocalTokenDesc(const QString& tokenName) {
    QString n = tokenName.toUpper().trimmed();
    if (n == "AGRICULTURE") return "Agriculture: +6 coins, +4 VP.";
    if (n == "ARCHITECTURE") return "Architecture: Wonders cost -2 resources.";
    if (n == "ECONOMY") return "Economy: Gain money when opponent trades.";
    if (n == "LAW") return "Law: +1 Scientific Symbol.";
    if (n == "MASONRY") return "Masonry: Blue cards cost -2 resources.";
    if (n == "MATHEMATICS") return "Mathematics: 3 VP per progress token.";
    if (n == "PHILOSOPHY") return "Philosophy: +7 VP.";
    if (n == "STRATEGY") return "Strategy: +1 Military Building.";
    if (n == "THEOLOGY") return "Theology: Wonders have 'Extra Turn' effect.";
    if (n == "URBANISM") return "Urbanism: +6 coins. Free chains.";
    return "Active bonus.";
}

void PlayerDashboardWidget::onEffectsClicked()
{
    // Build the list of effects
    QStringList effectsHtml;

    // 1. Progress Tokens
    if (!m_activeEffectsTokens.empty()) {
        effectsHtml << "<h3 style='color:#66BB6A'>✅ Progress Tokens</h3><ul>";
        for (const auto& t : m_activeEffectsTokens) {
             QString name = QString::fromStdString(t->getName());
             effectsHtml << "<li><b>" + name + ":</b> " + getLocalTokenDesc(name) + "</li>";
        }
        effectsHtml << "</ul>";
    }

    // 2. Built Wonders
    bool hasBuiltWonder = false;
    for (const auto& w : m_activeEffectsWonders) {
        if (w->getIsBuilt()) {
            if (!hasBuiltWonder) {
                effectsHtml << "<h3 style='color:#FFCA28'>✨ Built Wonders</h3><ul>";
                hasBuiltWonder = true;
            }
            QString name = QString::fromStdString(w->getName());
            QString eff = QString::fromStdString(w->getEffectDescription());
            effectsHtml << "<li><b>" + name + ":</b> " + eff + "</li>";
        }
    }
    if (hasBuiltWonder) effectsHtml << "</ul>";

    // 3. No effects?
    if (effectsHtml.isEmpty()) {
        effectsHtml << "<i>No permanent effects active.</i>";
    }

    // Display Dialog
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Active Effects");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(effectsHtml.join(""));
    msgBox.setStyleSheet(
        "QMessageBox { background-color: #3E2723; }"
        "QLabel { color: #F5E6D3; font-size: 14px; }"
        "QPushButton { background: #5D4037; color: white; padding: 5px 15px; border: 1px solid #8B4513; border-radius: 4px; }"
        "QPushButton:hover { background: #6D4C41; }"
    );
    msgBox.exec();
}