#include "SplashScreen.h"
#include "ui_SplashScreen.h"
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include <QGraphicsDropShadowEffect>

SplashScreen::SplashScreen(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SplashScreen)
{
    ui->setupUi(this);

    // Apply Shadows manually as they are tricky in .ui
    QGraphicsDropShadowEffect* titleShadow = new QGraphicsDropShadowEffect();
    titleShadow->setBlurRadius(20);
    titleShadow->setColor(QColor(0, 0, 0, 200));
    titleShadow->setOffset(4, 4);
    ui->titleLabel->setGraphicsEffect(titleShadow);

    QGraphicsDropShadowEffect* subtitleShadow = new QGraphicsDropShadowEffect();
    subtitleShadow->setBlurRadius(10);
    subtitleShadow->setColor(QColor(0, 0, 0, 180));
    subtitleShadow->setOffset(2, 2);
    ui->subtitleLabel->setGraphicsEffect(subtitleShadow);

    connect(ui->btnPvP, &QPushButton::clicked, [this]() { emit gameModeSelected(PvP); });
    connect(ui->btnPvA, &QPushButton::clicked, [this]() { emit gameModeSelected(PvAI); });
    connect(ui->btnAvA, &QPushButton::clicked, [this]() { emit gameModeSelected(AvAI); });

    // Focus to receive keyboard events
    this->setFocusPolicy(Qt::StrongFocus);
}

SplashScreen::~SplashScreen()
{
    delete ui;
}

void SplashScreen::keyPressEvent(QKeyEvent* event)
{
    // Key events disabled in favor of buttons
    QWidget::keyPressEvent(event);
}

void SplashScreen::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw decorative laurels
    QPen pen(QColor(218, 165, 32, 100)); // Transparent Gold
    pen.setWidth(3);
    painter.setPen(pen);

    // Top Left Laurel
    painter.drawArc(20, 20, 100, 100, 0, 90 * 16);
    // Top Right Laurel
    painter.drawArc(width() - 120, 20, 100, 100, 90 * 16, 90 * 16);
}