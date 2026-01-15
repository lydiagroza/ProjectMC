#pragma once

#include <QWidget>

namespace Ui {
class NameSelectionWidget;
}

class NameSelectionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NameSelectionWidget(QWidget *parent = nullptr);
    ~NameSelectionWidget();

    QString getPlayer1Name() const;
    QString getPlayer2Name() const;

signals:
    void namesConfirmed(const QString& p1, const QString& p2);

private slots:
    void onStartClicked();

private:
    Ui::NameSelectionWidget *ui;
};