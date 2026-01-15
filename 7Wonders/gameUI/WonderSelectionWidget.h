#pragma once
#include <QWidget>
#include <vector>
#include "CardWidget.h" 

namespace Ui {
class WonderSelectionWidget;
}

class WonderSelectionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WonderSelectionWidget(QWidget* parent = nullptr);
    ~WonderSelectionWidget();

    // Function to receive wonder data
    void setWonders(const std::vector<int>& ids, const std::vector<QString>& names, const std::vector<QString>& colors);

signals:
    void wonderChosen(int cardId);

private slots:
    void onCardClicked();

private:
    Ui::WonderSelectionWidget *ui;
    QList<CardWidget*> m_currentCards;
};
