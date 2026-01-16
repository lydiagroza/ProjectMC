#pragma once
#include <QWidget>
#include <vector>
#include <QString>
#include <QScrollArea> // Required for QScrollArea
#include <QHBoxLayout> // Required for QHBoxLayout

class CardWidget; 

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
    void setWonders(const std::vector<int>& ids, 
                    const std::vector<QString>& names, 
                    const std::vector<QString>& colors,
                    const std::vector<QString>& costs,
                    const std::vector<QString>& effects);

signals:
    void wonderChosen(int cardId);

private slots:
    void onCardClicked(int cardId);

private:
    Ui::WonderSelectionWidget *ui;
    QList<CardWidget*> m_currentCards;
};
