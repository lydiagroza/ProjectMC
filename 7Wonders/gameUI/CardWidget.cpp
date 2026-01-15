#include "CardWidget.h"
#include "ui_CardWidget.h"

CardWidget::CardWidget(int cardId, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CardWidget)
    , m_cardId(cardId)
    , m_isFaceUp(false)
{
    ui->setupUi(this);
    this->setFixedSize(70, 100);

    // Initial font setup
    QFont font = ui->cardButton->font();
    font.setBold(true);
    font.setPointSize(7);
    ui->cardButton->setFont(font);

    connect(ui->cardButton, &QPushButton::clicked, this, &CardWidget::onButtonClicked);
}

CardWidget::~CardWidget()
{
    delete ui;
}

void CardWidget::onButtonClicked()
{
    emit clicked();
}

void CardWidget::setupCard(const QString& name, const QString& colorCode, bool isFaceUp)
{
    m_isFaceUp = isFaceUp;

    if (m_isFaceUp) {
        ui->cardButton->setText(name);
        ui->cardButton->setEnabled(true);

        QString style = QString(
            "QPushButton { "
            "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
            "    stop:0 %1, stop:0.5 %2, stop:1 %1); "
            "  color: #F5E6D3; "
            "  border: 3px solid #8B4513; "
            "  border-radius: 8px; "
            "  padding: 5px; "
            "  font-family: 'Times New Roman', serif; "
            "  font-weight: bold; "
            "}"
            "QPushButton:hover { "
            "  border: 3px solid #DAA520; "
            "}"
        ).arg(colorCode, adjustBrightness(colorCode, 0.7));

        ui->cardButton->setStyleSheet(style);
        
        // Add shadow programmatically to the button
        QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(4);
        shadow->setColor(QColor(0, 0, 0, 150));
        shadow->setOffset(1, 1);
        ui->cardButton->setGraphicsEffect(shadow);
    }
    else {
        ui->cardButton->setText("?");
        ui->cardButton->setEnabled(false);

        ui->cardButton->setStyleSheet(
            "QPushButton { "
            "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
            "    stop:0 #8B7355, stop:0.5 #A0826D, stop:1 #8B7355); "
            "  color: #3E2723; "
            "  border: 3px solid #5D4037; "
            "  border-radius: 8px; "
            "  font-size: 20px; "
            "  font-family: 'Times New Roman'; "
            "}"
        );
    }
}

void CardWidget::setSelected(bool selected)
{
    if (!m_isFaceUp) return; 

    if (selected) {
        QString currentStyle = ui->cardButton->styleSheet();
        currentStyle.replace("border: 2px solid #333;", "border: 4px solid #f1c40f;");
        // Also catch the default style
        currentStyle.replace("border: 3px solid #8B4513;", "border: 4px solid #f1c40f;");
        ui->cardButton->setStyleSheet(currentStyle);
    }
    else {
        QString currentStyle = ui->cardButton->styleSheet();
        currentStyle.replace("border: 4px solid #f1c40f;", "border: 3px solid #8B4513;");
        ui->cardButton->setStyleSheet(currentStyle);
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