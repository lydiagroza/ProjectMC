#include "CardWidget.h"
#include "ui_CardWidget.h"
#include "CardDescriptions.h"
#include <QVBoxLayout>
#include <QFile>

CardWidget::CardWidget(int cardId, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CardWidget)
    , m_cardId(cardId)
    , m_isFaceUp(false)
    , m_popup(nullptr)
    , m_cardName("")
    , m_isWonder(false)
{
    ui->setupUi(this);
    this->setFixedSize(100, 140);
    setMouseTracking(true); 
    QFont font = ui->nameLabel->font();
    font.setBold(true);
    font.setPointSize(7); 
    ui->nameLabel->setFont(font);
    ui->overlayButton->setStyleSheet("background-color: transparent; border: none;");

    connect(ui->overlayButton, &QPushButton::clicked, this, &CardWidget::onButtonClicked);
}

CardWidget::~CardWidget()
{
    delete ui;
}

void CardWidget::onButtonClicked()
{
    if (m_popup) {
        m_popup->close();
        m_popup = nullptr;
    }
    emit cardClicked(m_cardId);
}

void CardWidget::enterEvent(QEnterEvent* event)
{
    if (!m_isFaceUp || m_isWonder) return;

    if (!m_popup) {
        m_popup = new CardInfoPopup();
    }

    QString description = CardDescriptions::getDescription(m_cardName);
    QPixmap cardImage = this->grab(); 

    m_popup->setCardData(m_cardName, description, cardImage);

    QPoint globalPos = this->mapToGlobal(this->rect().topRight());
    globalPos.setX(globalPos.x() + 5);
    m_popup->move(globalPos);
    m_popup->show();

    QWidget::enterEvent(event);
}

void CardWidget::leaveEvent(QEvent* event)
{
    if (m_popup) {
        m_popup->close();
        m_popup = nullptr;
    }
    QWidget::leaveEvent(event);
}


QString getSymbolEmoji(const QString& symbolName) {
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
    if (lowerSymbolName == "mortar") return "🥣";
    if (lowerSymbolName == "wheel") return "⚙️";
    return "";
}

void CardWidget::setupCard(const QString& name, const QString& colorCode, bool isFaceUp, const QString& cost, const QString& effect, const QString& unlocks)
{
    m_isFaceUp = isFaceUp;
    m_cardName = name;


    if (m_isFaceUp) {
        ui->nameLabel->setText(name);
        ui->nameLabel->setVisible(true);
        ui->costLabel->setText(cost);
        ui->costLabel->setVisible(!cost.isEmpty());
        QString finalEffect = effect;
        if (name.toUpper() == "DISPENSARY") {
            finalEffect += " " + getSymbolEmoji("mortar");
        }
        else if (name.toUpper() == "SCHOOL") {
            finalEffect += " " + getSymbolEmoji("wheel");
        }
        ui->effectLabel->setText(finalEffect);
        ui->effectLabel->setVisible(true);
        
        ui->symbolLabel->setText(getSymbolEmoji(unlocks));
        ui->symbolLabel->setVisible(!unlocks.isEmpty());

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
        ui->symbolLabel->setVisible(false);
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
        if (currentStyle.contains("border: 2px solid #333")) {
            currentStyle.replace("border: 2px solid #333", "border: 4px solid #f1c40f");
        } else {
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

void CardWidget::setImage(const QString& imagePath)
{
    m_isWonder = true;
    QPixmap pixmap(imagePath);
    if (!pixmap.isNull()) {
        m_cardName = ui->nameLabel->text();
  
        QString style = QString(
            "QFrame#cardFrame { "
            "  border-image: url(%1) 0 0 0 0 stretch stretch; "
            "  border: 3px solid #8B4513; "
            "  border-radius: 8px; "
            "}"
        ).arg(imagePath);
        ui->cardFrame->setStyleSheet(style);

     
        QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui->cardFrame->layout());
        if (layout) {
            layout->setAlignment(Qt::AlignTop);
            layout->removeWidget(ui->nameLabel);
            layout->insertWidget(0, ui->nameLabel);
            layout->removeWidget(ui->costLabel);
            layout->insertWidget(1, ui->costLabel);
        }

        QString baseStyle = "QLabel { "
                            "  background-color: transparent; "
                            "  color: black; "
                            "  font-family: 'Times New Roman', serif; "
                            "  font-weight: bold; "
                            "}";

        ui->nameLabel->setStyleSheet("QLabel { "
                                     "  background-color: transparent; "
                                     "  color: white; "
                                     "  font-family: 'Times New Roman', serif; "
                                     "  font-weight: bold; "
                                     "  font-size: 16px; "
                                     "}");
        ui->nameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

        QString infoStyle = "QLabel { "
                            "  background-color: transparent; "
                            "  color: black; "
                            "  font-family: 'Times New Roman', serif; "
                            "  font-weight: bold; "
                            "  font-size: 14px; "
                            "  padding-left: 5px; "
                            "}";
        ui->costLabel->setStyleSheet(infoStyle);
        ui->costLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        
        ui->effectLabel->setStyleSheet(infoStyle);
        ui->effectLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);

        QGraphicsDropShadowEffect* titleShadow = new QGraphicsDropShadowEffect();
        titleShadow->setColor(QColor(0, 0, 0, 255));
        titleShadow->setBlurRadius(4);
        titleShadow->setOffset(1, 1);
        ui->nameLabel->setGraphicsEffect(titleShadow);

        auto addWhiteGlow = [](QWidget* w) {
            QGraphicsDropShadowEffect* glow = new QGraphicsDropShadowEffect();
            glow->setColor(QColor(255, 255, 255, 220));
            glow->setBlurRadius(4);
            glow->setOffset(0, 0);
            w->setGraphicsEffect(glow);
        };

        addWhiteGlow(ui->costLabel);
        addWhiteGlow(ui->effectLabel);

        ui->nameLabel->setVisible(true);
        ui->costLabel->setVisible(!ui->costLabel->text().isEmpty());
        ui->effectLabel->setVisible(true); 

    }
}

QString CardWidget::getWonderImagePath(const QString& wonderName)
{
    QString normalized = wonderName.simplified().remove(' ').toUpper();
    
    if (normalized == "THEPYRAMIDS") normalized = "THEPIRAMIDS";
    if (normalized == "HANGINGGARDENS") normalized = "HANGINGGARDEN";
    if (normalized == "THEHANGINGGARDENS") normalized = "HANGINGGARDEN";
    if (normalized == "THEHANGINGGARDEN") normalized = "HANGINGGARDEN";
    if (normalized == "THETEMPLEOFARTEMIS") normalized = "THEGREATTEMPLEOFARTEMIS";
    if (normalized == "TEMPLEOFARTEMIS") normalized = "THEGREATTEMPLEOFARTEMIS";
    if (normalized == "THEAPPIANWAY") normalized = "THEAPIANWAY"; 
    if (normalized == "APPIANWAY") normalized = "THEAPIANWAY";
    
    QString filename = normalized + ".png";
    QStringList searchPaths = {
        "UI/" + filename,           // Project root
        "../UI/" + filename,        // One level up
        "../../UI/" + filename,     // Two levels up (common for build folders)
        ":/wonders/UI/" + filename  // Fallback to resource if any left
    };

    for (const QString& path : searchPaths) {
        if (QFile::exists(path)) {
            return path;
        }
    }
    
    return "";
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
