#pragma once

#include <QDialog>
#include <vector>
#include "Wonder.h"

class Player;

namespace Ui {
class WonderChoiceDialog;
}

class WonderChoiceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WonderChoiceDialog(QWidget *parent = nullptr);
    ~WonderChoiceDialog();

    void setWonders(const std::vector<Wonder*>& wonders, Player* player, Player* opponent);
    int getSelectedWonderId() const { return m_selectedWonderId; }

private slots:
    void onCardClicked();

private:
    Ui::WonderChoiceDialog *ui;
    int m_selectedWonderId = -1;
};