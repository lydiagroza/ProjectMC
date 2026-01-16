#pragma once

#include <QDialog>
#include "Game.h"

namespace Ui {
class GameEndDialog;
}

class GameEndDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GameEndDialog(Game* game, QWidget *parent = nullptr);
    ~GameEndDialog();

private:
    Ui::GameEndDialog *ui;
    void setupStats(Game* game);
    QString formatStats(const Player& p, const Player& opp);
};
