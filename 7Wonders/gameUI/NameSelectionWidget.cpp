#include "NameSelectionWidget.h"
#include "ui_NameSelectionWidget.h"
#include <QGraphicsDropShadowEffect>

NameSelectionWidget::NameSelectionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NameSelectionWidget)
{
    ui->setupUi(this);

    // Apply shadow to title
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(8);
    shadow->setColor(QColor(0, 0, 0, 200));
    shadow->setOffset(2, 2);
    ui->titleLabel->setGraphicsEffect(shadow);

    connect(ui->btnStart, &QPushButton::clicked, this, &NameSelectionWidget::onStartClicked);

    // Set defaults
    ui->inputP1->setText("Player 1");
    ui->inputP2->setText("Player 2");
}

NameSelectionWidget::~NameSelectionWidget()
{
    delete ui;
}

void NameSelectionWidget::setMode(bool p1Enabled, bool p2Enabled)
{
    ui->inputP1->setEnabled(p1Enabled);
    ui->inputP2->setEnabled(p2Enabled);

    if (!p1Enabled) ui->inputP1->setText("AI Player 1");
    else ui->inputP1->setText("Player 1");

    if (!p2Enabled) ui->inputP2->setText("AI Player 2");
    else ui->inputP2->setText("Player 2");
}

QString NameSelectionWidget::getPlayer1Name() const
{
    return ui->inputP1->text().trimmed();
}

QString NameSelectionWidget::getPlayer2Name() const
{
    return ui->inputP2->text().trimmed();
}

void NameSelectionWidget::onStartClicked()
{
    QString p1 = getPlayer1Name();
    QString p2 = getPlayer2Name();

    if (p1.isEmpty()) p1 = "Player 1";
    if (p2.isEmpty()) p2 = "Player 2";

    emit namesConfirmed(p1, p2);
}