#include "MilitaryTrackWidget.h"
#include <QHBoxLayout>

MilitaryTrackWidget::MilitaryTrackWidget(QWidget* parent)
    : QWidget(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);

    // A container to allow the pawn to float over the track segments
    QFrame* trackContainer = new QFrame(this);
    trackContainer->setObjectName("trackContainer");
    m_mainLayout->addWidget(trackContainer);

    createTrack();

    // Create the pawn
    m_pawn = new QLabel(trackContainer);
    m_pawn->setFixedSize(18, 18);
    m_pawn->setStyleSheet(
        "background-color: #e74c3c;"
        "border-radius: 9px;"
        "border: 2px solid white;"
    );
    m_pawn->show();

    // Set initial position
    updatePawnPosition(0);
}

void MilitaryTrackWidget::createTrack()
{
    QWidget* trackWidget = findChild<QWidget*>("trackContainer");
    if (!trackWidget) return;

    QVBoxLayout* trackLayout = new QVBoxLayout(trackWidget);
    trackLayout->setSpacing(1);
    trackLayout->setContentsMargins(0, 0, 0, 0);

    m_trackSegments.resize(m_totalSegments);

    for (int i = m_totalSegments - 1; i >= 0; --i) {
        int position = i - 9; // Map index to position (-9 to 9)
        QFrame* segment = new QFrame(this);
        segment->setMinimumHeight(m_segmentHeight);
        segment->setFrameShape(QFrame::Box);

        QString style = "background-color: rgba(0,0,0,0.2);";
        if (position == 9 || position == -9) {
            style += "background-color: #c0392b;"; // Victory zones
        } else if (position == 2 || position == 5 || position == -2 || position == -5) {
            style += "border-top: 1px dashed #f1c40f;"; // Token zones
        }

        segment->setStyleSheet(style);
        trackLayout->addWidget(segment);
        m_trackSegments[i] = segment;
    }
}

void MilitaryTrackWidget::updatePawnPosition(int position)
{
    // Clamp position to be within -9 and 9
    position = std::max(-9, std::min(9, position));

    // Convert game position (-9 to 9) to layout index (0 to 18)
    int index = position + 9;

    // The layout is reversed, so we calculate from the bottom
    int yPos = (m_totalSegments - 1 - index) * (m_segmentHeight + 1) + (m_segmentHeight / 2) - (m_pawn->height() / 2);
    int xPos = (this->width() / 2) - (m_pawn->width() / 2);

    m_pawn->move(xPos, yPos);
}
