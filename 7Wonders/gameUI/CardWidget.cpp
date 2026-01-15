#include "CardWidget.h"
#include "ui_CardWidget.h"

CardWidget::CardWidget(int cardId, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CardWidget)
    , m_cardId(cardId)
    , m_isFaceUp(false)
{
    ui->setupUi(this);
    this->setFixedSize(100, 140);

    // Initial font setup
    QFont font = ui->nameLabel->font();
    font.setBold(true);
    font.setPointSize(8);
    ui->nameLabel->setFont(font);

    // Ensure overlay button is transparent
    ui->overlayButton->setStyleSheet("background-color: transparent; border: none;");

    connect(ui->overlayButton, &QPushButton::clicked, this, &CardWidget::onButtonClicked);
}

CardWidget::~CardWidget()
{
    delete ui;
}

void CardWidget::onButtonClicked()
{
    emit clicked();
}

void CardWidget::setupCard(const QString& name, const QString& colorCode, bool isFaceUp, const QString& cost, const QString& effect)
{
    m_isFaceUp = isFaceUp;

    if (m_isFaceUp) {
        ui->nameLabel->setText(name);
        ui->nameLabel->setVisible(true);
        ui->costLabel->setText(cost);
        ui->costLabel->setVisible(!cost.isEmpty());
        ui->effectLabel->setText(effect);
        ui->effectLabel->setVisible(true);

        QString borderStyle = QString(
            "QFrame#cardFrame { "
            "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
            "    stop:0 %1, stop:0.4 %2, stop:1 %1); "
            "  border: 2px solid #333; "
            "  border-radius: 8px; "
            "}"
        ).arg(colorCode, adjustBrightness(colorCode, 0.8));

        ui->cardFrame->setStyleSheet(borderStyle);
        ui->overlayButton->setEnabled(true);
        
        // Add shadow
        QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(6);
        shadow->setColor(QColor(0, 0, 0, 120));
        shadow->setOffset(2, 2);
        ui->cardFrame->setGraphicsEffect(shadow);
    }
    else {
        // Face down (Back of card)
        ui->nameLabel->setVisible(false);
        ui->costLabel->setVisible(false);
        ui->effectLabel->setText("?");
        ui->effectLabel->setStyleSheet("color: #3E2723; background: transparent; font-size: 30px; font-weight: bold; border: none;");
        
        ui->cardFrame->setStyleSheet(
            "QFrame#cardFrame { "
            "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
            "    stop:0 #8B7355, stop:0.5 #A0826D, stop:1 #8B7355); "
            "  border: 3px solid #5D4037; "
            "  border-radius: 8px; "
            "}"
        );
        ui->overlayButton->setEnabled(false);
    }
}

void CardWidget::setSelected(bool selected)
{
    if (!m_isFaceUp) return; 

    QString currentStyle = ui->cardFrame->styleSheet();
    if (selected) {
        // Replace border color
        // Note: This is a bit brittle string replacement, but works for the set style
        if (currentStyle.contains("border: 2px solid #333")) {
            currentStyle.replace("border: 2px solid #333", "border: 4px solid #f1c40f");
        } else {
             // Fallback
             currentStyle += "QFrame#cardFrame { border: 4px solid #f1c40f; }";
        }
        ui->cardFrame->setStyleSheet(currentStyle);
    }
    else {
        if (currentStyle.contains("border: 4px solid #f1c40f")) {
            currentStyle.replace("border: 4px solid #f1c40f", "border: 2px solid #333");
        }
        ui->cardFrame->setStyleSheet(currentStyle);
    }
}

QString CardWidget::adjustBrightness(const QString& hexColor, double factor)
{
    QString hex = hexColor;
    if (hex.startsWith("#")) {
        hex = hex.mid(1);
    }

    bool ok;
    int r = hex.mid(0, 2).toInt(&ok, 16);
    int g = hex.mid(2, 2).toInt(&ok, 16);
    int b = hex.mid(4, 2).toInt(&ok, 16);

    r = qBound(0, static_cast<int>(r * factor), 255);
    g = qBound(0, static_cast<int>(g * factor), 255);
    b = qBound(0, static_cast<int>(b * factor), 255);

    return QString("#%1%2%3")
        .arg(r, 2, 16, QChar('0'))
        .arg(g, 2, 16, QChar('0'))
        .arg(b, 2, 16, QChar('0'))
        .toUpper();
}