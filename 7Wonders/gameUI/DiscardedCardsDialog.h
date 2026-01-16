#pragma once

#include <QDialog>
#include <vector>
#include "CardBase.h"

namespace Ui {
class DiscardedCardsDialog;
}

class DiscardedCardsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DiscardedCardsDialog(const std::vector<const CardBase*>& discardedCards, QWidget *parent = nullptr);
    ~DiscardedCardsDialog();

private:
    void setupCards(const std::vector<const CardBase*>& discardedCards);

    Ui::DiscardedCardsDialog *ui;
};
