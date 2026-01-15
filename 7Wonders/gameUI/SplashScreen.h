#pragma once

#include <QWidget>

namespace Ui {
class SplashScreen;
}

QT_BEGIN_NAMESPACE
class QKeyEvent;
class QPaintEvent;
QT_END_NAMESPACE

class SplashScreen : public QWidget
{
    Q_OBJECT

public:
    enum GameMode { PvP, PvAI, AvAI };
    explicit SplashScreen(QWidget* parent = nullptr);
    ~SplashScreen();

signals:
    void gameModeSelected(GameMode mode);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    Ui::SplashScreen *ui;
};