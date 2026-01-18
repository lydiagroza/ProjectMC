#include "MilitaryTrackWidget.h"
#include "ui_MilitaryTrackWidget.h"
#include "SoundManager.h"
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QResizeEvent>

MilitaryTrackWidget::MilitaryTrackWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MilitaryTrackWidget)
{
    ui->setupUi(this);

    createTrack();

    //Create the pawn
    m_pawn = new QLabel(ui->trackContainer);
    m_pawn->setFixedSize(30, 30); 
    m_pawn->setText("🗡️"); 
    m_pawn->setAlignment(Qt::AlignCenter);
    m_pawn->setStyleSheet(
        "background: qradialgradient(cx:0.5, cy:0.5, radius: 0.5, fx:0.5, fy:0.5, stop:0 #CFD8DC, stop:1 #90A4AE);" // Silver/Steel look
        "border: 2px solid #455A64;"
        "border-radius: 15px;"
        "font-size: 18px;"
    );
    
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(10);
    shadow->setColor(QColor(0, 0, 0, 200));
    shadow->setOffset(0, 3);
    m_pawn->setGraphicsEffect(shadow);

    m_pawn->show();
    m_pawn->raise(); 
    updatePawnPosition(0);
}

MilitaryTrackWidget::~MilitaryTrackWidget()
{
    delete ui;
}

void MilitaryTrackWidget::createTrack()
{
    QVBoxLayout* trackLayout = qobject_cast<QVBoxLayout*>(ui->trackContainer->layout());
    if (!trackLayout) return;

    QLayoutItem* item;
    while ((item = trackLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    m_trackSegments.resize(m_totalSegments);

    for (int i = m_totalSegments - 1; i >= 0; --i) {
        int position = i - 9; // Map index to position (-9 to 9)
        QFrame* segment = new QFrame(this);
        segment->setMinimumHeight(m_segmentHeight);
        segment->setFrameShape(QFrame::StyledPanel);
        
    
        QString baseStyle = 
            "border: 1px solid #3E2723; "
            "margin-left: 5px; margin-right: 5px; "
            "border-radius: 3px; "; 

        if (position == 0) {
            baseStyle += "background: qlinear-gradient(x1:0, y1:0, x2:1, y2:0, stop:0 #F9A825, stop:0.5 #FDD835, stop:1 #F9A825); border: 2px solid #FFD700;"; 
        } 
        else if (abs(position) == 9) {
            baseStyle += "background: qlinear-gradient(x1:0, y1:0, x2:1, y2:0, stop:0 #B71C1C, stop:0.5 #D32F2F, stop:1 #B71C1C); border: 2px solid #FFD700;";
        }
        else if (abs(position) >= 6) {
             baseStyle += "background: #8D6E63;";
        }
        else if (abs(position) >= 3) {
             baseStyle += "background: #A1887F;";
        }
        else {
             baseStyle += "background: #D7CCC8;";
        }

        if (position == 2 || position == 5) {
            baseStyle += "border-top: 2px dashed #8B0000;";
        } else if (position == -2 || position == -5) {
            baseStyle += "border-bottom: 2px dashed #8B0000;";
        }

        segment->setStyleSheet(baseStyle);
        trackLayout->addWidget(segment);
        m_trackSegments[i] = segment;
    }
}

void MilitaryTrackWidget::updatePawnPosition(int position)
{
    position = std::max(-9, std::min(9, position));

    if (m_currentPosition != position) {
        SoundManager::instance().playSword();
    }
    m_currentPosition = position;

    int index = position + 9;
    
    QFrame* targetSegment = m_trackSegments[index];
    int targetY = 0;
    
    if (targetSegment && targetSegment->height() > 0) {
        targetY = targetSegment->y() + (targetSegment->height() / 2) - (m_pawn->height() / 2);
    } else {
        int layoutIndex = m_totalSegments - 1 - index;
        targetY = (layoutIndex * m_segmentHeight) + (m_segmentHeight/2) - (m_pawn->height()/2);
    }
    
    int xPos = (ui->trackContainer->width() / 2) - (m_pawn->width() / 2);

    m_pawn->move(xPos, targetY);
}

void MilitaryTrackWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updatePawnPosition(m_currentPosition);
}