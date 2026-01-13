#pragma once
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <vector>
#include "CardWidget.h" // Refolosim clasa ta de c?r?i!

class WonderSelectionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WonderSelectionWidget(QWidget* parent = nullptr);

    // Func?ia care prime?te datele celor 4 minuni
    void setWonders(const std::vector<int>& ids, const std::vector<QString>& names, const std::vector<QString>& colors);

signals:
    void wonderChosen(int cardId); // Semnal când alegi o minune

private slots:
    void onCardClicked(); // Ce se întâmpl? când dai click

private:
    QLabel* m_title;
    QWidget* m_cardsContainer;
    QGridLayout* m_layout;
    QList<CardWidget*> m_currentCards;
};