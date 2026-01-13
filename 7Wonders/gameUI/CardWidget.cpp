#include "CardWidget.h"

CardWidget::CardWidget(int cardId, QWidget* parent)
    : QPushButton(parent), m_cardId(cardId), m_isFaceUp(false)
{
    this->setFixedSize(70, 100);
    this->setFlat(false);

    QFont font = this->font();
    font.setBold(true);
    font.setPointSize(7);
    this->setFont(font);
}

void CardWidget::setupCard(const QString& name, const QString& colorCode, bool isFaceUp)
{
    m_isFaceUp = isFaceUp;

    if (m_isFaceUp) {
        this->setText(name);
        this->setEnabled(true);

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

        this->setStyleSheet(style);
        
        // ADAUGĂ shadow effect programatic
        QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(4);
        shadow->setColor(QColor(0, 0, 0, 150));
        shadow->setOffset(1, 1);
        this->setGraphicsEffect(shadow);
    }
    else {
        this->setText("?");
        this->setEnabled(false);

        this->setStyleSheet(
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
   
        QString currentStyle = this->styleSheet();
        currentStyle.replace("border: 2px solid #333;", "border: 4px solid #f1c40f;");
        this->setStyleSheet(currentStyle);
    }
    else {
      
        QString currentStyle = this->styleSheet();
        currentStyle.replace("border: 4px solid #f1c40f;", "border: 2px solid #333;");
        this->setStyleSheet(currentStyle);
    }
}
QString CardWidget::adjustBrightness(const QString& hexColor, double factor)
{
    // Elimină # din hex
    QString hex = hexColor;
    if (hex.startsWith("#")) {
        hex = hex.mid(1);
    }

    // Convertește hex în RGB
    bool ok;
    int r = hex.mid(0, 2).toInt(&ok, 16);
    int g = hex.mid(2, 2).toInt(&ok, 16);
    int b = hex.mid(4, 2).toInt(&ok, 16);

    // Ajustează luminozitatea
    r = qBound(0, static_cast<int>(r * factor), 255);
    g = qBound(0, static_cast<int>(g * factor), 255);
    b = qBound(0, static_cast<int>(b * factor), 255);

    // Returnează hex-ul nou
    return QString("#%1%2%3")
        .arg(r, 2, 16, QChar('0'))
        .arg(g, 2, 16, QChar('0'))
        .arg(b, 2, 16, QChar('0'))
        .toUpper();
}