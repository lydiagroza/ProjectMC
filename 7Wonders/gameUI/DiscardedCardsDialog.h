#pragma once

#include <QDialog>
#include <vector>

class CardBase;
class Ui_DiscardedCardsDialog;

class DiscardedCardsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DiscardedCardsDialog(const std::vector<const CardBase*>& discardedCards, QWidget *parent = nullptr);
    ~DiscardedCardsDialog();

    int getSelectedCardId() const;

signals:
    void cardSelected(int cardId);

private slots:
    void onCardClicked(int cardId);

private:
    void setupCards(const std::vector<const CardBase*>& discardedCards);

    Ui_DiscardedCardsDialog *ui;
    int m_selectedCardId;
};
