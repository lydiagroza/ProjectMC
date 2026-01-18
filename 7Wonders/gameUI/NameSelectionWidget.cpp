#include "NameSelectionWidget.h"
#include "ui_NameSelectionWidget.h"
#include "SoundManager.h"
#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>
#include <QLabel> 

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

    m_difficultyCombo = new QComboBox(this);
    m_difficultyCombo->addItem("Easy (Random)", 0);
    m_difficultyCombo->addItem("Medium (Heuristic)", 1);
    m_difficultyCombo->addItem("Hard (ML Trained)", 2);
    m_difficultyCombo->addItem("Adaptive (ML Live)", 3);
    m_difficultyCombo->setCurrentIndex(1); // Default Medium

    m_difficultyCombo->setStyleSheet(
        "QComboBox { "
        "  background-color: #3E2723; "
        "  color: #FFD700; "
        "  border: 2px solid #8B4513; "
        "  border-radius: 5px; "
        "  padding: 5px; "
        "  font-family: 'Times New Roman'; "
        "  font-size: 16px; "
        "}"
        "QComboBox::drop-down { border: 0px; }"
    );
    
    if (this->layout()) {
        m_difficultyLabel = new QLabel("Select AI Difficulty:", this); 
        m_difficultyLabel->setStyleSheet("color: #F5E6D3; font-weight: bold; font-size: 14px;");
        m_difficultyLabel->setAlignment(Qt::AlignCenter);
        
    
        QVBoxLayout* vLayout = qobject_cast<QVBoxLayout*>(this->layout());
        if (vLayout) {
            vLayout->insertWidget(vLayout->count() - 1, m_difficultyLabel);
            vLayout->insertWidget(vLayout->count() - 1, m_difficultyCombo);
        } else {
            this->layout()->addWidget(m_difficultyLabel);
            this->layout()->addWidget(m_difficultyCombo);
        }
    }
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
    
    // Enable difficulty only if there is at least one AI
    bool hasAI = (!p1Enabled || !p2Enabled);
    m_difficultyCombo->setVisible(hasAI);
    m_difficultyLabel->setVisible(hasAI); 
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
    SoundManager::instance().playQuack();

    QString p1 = getPlayer1Name();
    QString p2 = getPlayer2Name();

    if (p1.isEmpty()) p1 = "Player 1";
    if (p2.isEmpty()) p2 = "Player 2";

    int diff = m_difficultyCombo->currentIndex();
    emit namesConfirmed(p1, p2, diff);
}