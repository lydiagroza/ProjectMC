#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <vector>

namespace Ui {
class MilitaryTrackWidget;
}

class MilitaryTrackWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MilitaryTrackWidget(QWidget* parent = nullptr);
    ~MilitaryTrackWidget();

    // Updates the pawn's position on the track.
    // Position ranges from -9 (Player 2 max) to +9 (Player 1 max).
    void updatePawnPosition(int position);

private:
    void createTrack();

    Ui::MilitaryTrackWidget *ui;
    std::vector<QFrame*> m_trackSegments;
    QLabel* m_pawn;
    const int m_segmentHeight = 20;
    const int m_totalSegments = 19; // from -9 to +9
};