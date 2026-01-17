#pragma once

#include <QWidget>
#include <QComboBox>
#include <QLabel> // Added missing include

namespace Ui {
class NameSelectionWidget;
}

class NameSelectionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NameSelectionWidget(QWidget *parent = nullptr);
    ~NameSelectionWidget();

    void setMode(bool p1Enabled, bool p2Enabled);
    QString getPlayer1Name() const;
    QString getPlayer2Name() const;

signals:
    void namesConfirmed(const QString& p1, const QString& p2, int difficulty);

private slots:
    void onStartClicked();

private:
    Ui::NameSelectionWidget *ui;
    QComboBox* m_difficultyCombo;
    QLabel* m_difficultyLabel; // New member variable
};