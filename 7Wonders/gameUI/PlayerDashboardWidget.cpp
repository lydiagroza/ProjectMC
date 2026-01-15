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

void PlayerDashboardWidget::updateDashboard(const std::string& name, int coins, const std::vector<std::shared_ptr<Wonder>>& wonders)
{
    // Update Text
    QString icon = (this->styleSheet().contains("#8B0000")) ? "⚔️" : "🏛️";
    QString text = QString("%1 %2 | 🦆 %3 coins")
        .arg(icon)
        .arg(QString::fromStdString(name))
        .arg(coins);
    ui->infoLabel->setText(text);

    // Update Wonders
    // Clear existing items in layout
    QLayoutItem* item;
    while ((item = ui->wondersLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Add new wonder widgets
    for (const auto& wonderPtr : wonders) {
        CardWidget* wonderWidget = new CardWidget(wonderPtr->getId(), ui->wondersContainer);
        wonderWidget->setFixedSize(100, 140);

        QString wName = QString::fromStdString(wonderPtr->getName());
        QString color = wonderPtr->getIsBuilt() ? "#DAA520" : "#6D4C41";
        QString effect = QString::fromStdString(wonderPtr->getEffectDescription());

        wonderWidget->setupCard(wName, color, true, "", effect);

        if (wName.trimmed().toLower() == "the statue of zeus") {
            wonderWidget->setImage(":/wonders/UI/THESTATUEOFZEUS.png");
        }

        ui->wondersLayout->addWidget(wonderWidget);
    }
    
    // Add stretch to keep them aligned left (or center if you prefer)
    ui->wondersLayout->addStretch();
}
