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
    void updatePawnPosition(int position);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void createTrack();

    Ui::MilitaryTrackWidget *ui;
    std::vector<QFrame*> m_trackSegments;
    QLabel* m_pawn;
    int m_currentPosition = 0;
    const int m_segmentHeight = 20;
    const int m_totalSegments = 19; // from -9 to +9
};