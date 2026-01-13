#include "SplashScreen.h"
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include <QGraphicsDropShadowEffect>

SplashScreen::SplashScreen(QWidget* parent)
    : QWidget(parent)
{
    // Layout vertical centrat
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(30);

    // Background cu gradient antic
    this->setStyleSheet(
        "QWidget { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #2C1810, stop:0.3 #3E2723, stop:0.6 #2C1810, stop:1 #1A0F0A); "
        "}"
    );

    // ============= TITLU PRINCIPAL =============
    m_titleLabel = new QLabel("⚔️ 7 WONDERS DUEL ⚔️", this);
    m_titleLabel->setStyleSheet(
        "QLabel { "
        "  color: #FFD700; "
        "  font-family: 'Trajan Pro', 'Times New Roman', serif; "
        "  font-size: 72px; "
        "  font-weight: bold; "
        "  background: transparent; "
        "  letter-spacing: 3px; "
        "  text-align: center; "
        "}"
    );
    m_titleLabel->setAlignment(Qt::AlignCenter);

    // Efect de umbră dramatică
    QGraphicsDropShadowEffect* titleShadow = new QGraphicsDropShadowEffect();
    titleShadow->setBlurRadius(20);
    titleShadow->setColor(QColor(0, 0, 0, 200));
    titleShadow->setOffset(4, 4);
    m_titleLabel->setGraphicsEffect(titleShadow);

    // ============= SUBTITLU =============
    m_subtitleLabel = new QLabel("~ The Empire of the Ducks ~", this);
    m_subtitleLabel->setStyleSheet(
        "QLabel { "
        "  color: #DAA520; "
        "  font-family: 'Times New Roman', serif; "
        "  font-size: 28px; "
        "  font-style: italic; "
        "  background: transparent; "
        "  letter-spacing: 2px; "
        "}"
    );
    m_subtitleLabel->setAlignment(Qt::AlignCenter);

    QGraphicsDropShadowEffect* subtitleShadow = new QGraphicsDropShadowEffect();
    subtitleShadow->setBlurRadius(10);
    subtitleShadow->setColor(QColor(0, 0, 0, 180));
    subtitleShadow->setOffset(2, 2);
    m_subtitleLabel->setGraphicsEffect(subtitleShadow);

    // ============= DECORAȚII ROMANE =============
    QLabel* decorTop = new QLabel("═══════════════════════════════", this);
    decorTop->setStyleSheet(
        "color: #8B4513; font-size: 24px; background: transparent;"
    );
    decorTop->setAlignment(Qt::AlignCenter);

    QLabel* decorBottom = new QLabel("═══════════════════════════════", this);
    decorBottom->setStyleSheet(
        "color: #8B4513; font-size: 24px; background: transparent;"
    );
    decorBottom->setAlignment(Qt::AlignCenter);

    // ============= MESAJ "PRESS ENTER" - FĂRĂ PÂLPÂIRE =============
    m_pressEnterLabel = new QLabel("⏎ PRESS ENTER TO BEGIN YOUR CONQUEST ⏎", this);
    m_pressEnterLabel->setStyleSheet(
        "QLabel { "
        "  color: #F5E6D3; "
        "  font-family: 'Times New Roman', serif; "
        "  font-size: 20px; "
        "  font-weight: bold; "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 transparent, stop:0.3 #8B4513, stop:0.7 #8B4513, stop:1 transparent); "
        "  padding: 15px; "
        "  border-radius: 10px; "
        "  border: 2px solid #DAA520; "
        "}"
    );
    m_pressEnterLabel->setAlignment(Qt::AlignCenter);

    // ✅ ELIMINAT: QTimer pentru pulsare - label-ul rămâne vizibil permanent

    // ============= ADAUGĂ ÎN LAYOUT =============
    layout->addStretch(1);
    layout->addWidget(decorTop);
    layout->addSpacing(20);
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_subtitleLabel);
    layout->addSpacing(20);
    layout->addWidget(decorBottom);
    layout->addStretch(2);
    layout->addWidget(m_pressEnterLabel);
    layout->addStretch(1);

    // Focus pentru a primi evenimente de tastatură
    this->setFocusPolicy(Qt::StrongFocus);
}

void SplashScreen::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        emit startGame();
    }
    QWidget::keyPressEvent(event);
}

void SplashScreen::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);

    // Poți adăuga decorații extra aici cu QPainter
    // De exemplu: coloane romane, lauri, etc.
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Desenează niște "lauri" decorativi în colțuri (opțional)
    QPen pen(QColor(218, 165, 32, 100)); // Auriu transparent
    pen.setWidth(3);
    painter.setPen(pen);

    // Laur stânga sus
    painter.drawArc(20, 20, 100, 100, 0, 90 * 16);
    // Laur dreapta sus
    painter.drawArc(width() - 120, 20, 100, 100, 90 * 16, 90 * 16);
}
