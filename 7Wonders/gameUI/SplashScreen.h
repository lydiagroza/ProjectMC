#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
class QKeyEvent;
class QPaintEvent;
QT_END_NAMESPACE

class SplashScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SplashScreen(QWidget* parent = nullptr);

signals:
    void startGame();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    QLabel* m_titleLabel;
    QLabel* m_subtitleLabel;
    QLabel* m_pressEnterLabel;
};

