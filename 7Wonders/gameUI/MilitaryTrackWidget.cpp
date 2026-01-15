#include "MilitaryTrackWidget.h"
#include "ui_MilitaryTrackWidget.h"
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>

MilitaryTrackWidget::MilitaryTrackWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MilitaryTrackWidget)
{
    ui->setupUi(this);

    // Dark Wood Background for the whole track area
    ui->trackContainer->setStyleSheet(
        "#trackContainer { "
        "  background-color: #3E2723; "
        "  border: 4px solid #1a100c; "
        "  border-radius: 8px; "
        "}"
    );

    createTrack();

    // Create the pawn
    m_pawn = new QLabel(ui->trackContainer);
    m_pawn->setFixedSize(30, 30); // Slightly larger
    m_pawn->setText("⚔️");
    m_pawn->setAlignment(Qt::AlignCenter);
    m_pawn->setStyleSheet(
        "background: qradialgradient(cx:0.5, cy:0.5, radius: 0.5, fx:0.5, fy:0.5, stop:0 #FFD700, stop:1 #B8860B);"
        "border: 2px solid #5D4037;"
        "border-radius: 15px;" // Circle
        "font-size: 16px;"
        "color: #3E2723;"
    );
    
    // Add shadow to pawn for depth
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(10);
    shadow->setColor(QColor(0, 0, 0, 200));
    shadow->setOffset(0, 3);
    m_pawn->setGraphicsEffect(shadow);

    m_pawn->show();
    m_pawn->raise(); // Ensure on top

    // Set initial position
    updatePawnPosition(0);
}

MilitaryTrackWidget::~MilitaryTrackWidget()
{
    delete ui;
}

void MilitaryTrackWidget::createTrack()
{
    // Access the layout already defined in .ui
    QVBoxLayout* trackLayout = qobject_cast<QVBoxLayout*>(ui->trackContainer->layout());
    if (!trackLayout) return;

    // Clear any design-time placeholders
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
        
        // Base Style (Parchment)
        QString style = 
            "border: 1px solid #5D4037; "
            "margin-left: 10px; margin-right: 10px; "; // Indent to look like a ladder

        if (position == 0) {
            // Neutral Center
            style += "background: #8D6E63; border: 2px solid #3E2723;"; 
        } 
        else if (abs(position) == 9) {
             // Victory Zone
            style += "background: #B71C1C; border: 2px solid #FFD700;";
        }
        else if (abs(position) >= 6) {
             // Danger Zone (High)
             style += "background: #E57373;";
        }
        else if (abs(position) >= 3) {
             // Danger Zone (Medium)
             style += "background: #FFCCBC;";
        }
        else {
             // Low Intensity
             style += "background: #D7CCC8;";
        }

        // Add markings for Token Loss (2 and 5)
        // These are boundaries. The segment itself is the space.
        // Let's mark the segment that represents the threshold? 
        // Or just change the border.
        if (abs(position) == 2 || abs(position) == 5) {
            style += "border-bottom: 2px dashed #D32F2F;";
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
    // We target the center of the segment
    // Using simple geometry
    
    // Calculate total height of the track container
    // It's safer to map directly to the segment widget's geometry if possible,
    // but they are in a layout.
    // However, since we stored m_trackSegments, we can use their geometry!
    
    // NOTE: We need to wait for layout to apply to get correct coordinates?
    // updatePawnPosition might be called before show().
    // Fallback to calculation if geometry is 0.
    
    QFrame* targetSegment = m_trackSegments[index];
    int targetY = 0;
    
    if (targetSegment && targetSegment->y() > 0) {
        // Use actual widget position
        targetY = targetSegment->y() + (targetSegment->height() / 2) - (m_pawn->height() / 2);
    } else {
        // Fallback calculation
        // Total segments: 19.
        int layoutIndex = m_totalSegments - 1 - index;
        targetY = layoutIndex * (m_segmentHeight + 2) + 10; // +2 for spacing/border approx
        // This fallback is brittle, but standard behavior in Qt creates usually handles this.
        // Better fallback: Just assume centered in the slot.
        int contentHeight = ui->trackContainer->height();
        if (contentHeight < 100) contentHeight = m_totalSegments * m_segmentHeight; // estimating
        
        targetY = (layoutIndex * m_segmentHeight) + (m_segmentHeight/2) - (m_pawn->height()/2);
    }
    
    int xPos = (ui->trackContainer->width() / 2) - (m_pawn->width() / 2);

    m_pawn->move(xPos, targetY);
}