#pragma once
#include <QWidget>
#include <QGraphicsDropShadowEffect>
#include <QEvent>
#include "CardInfoPopup.h"

namespace Ui {
class CardWidget;
}

class CardInfoPopup;

class CardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CardWidget(int cardId, QWidget* parent = nullptr);
    ~CardWidget();

    void setupCard(const QString& name, const QString& colorCode, bool isFaceUp, const QString& cost = "", const QString& effect = "", const QString& unlocks = "");
    void setSelected(bool selected);
    void setImage(const QString& imagePath);
    static QString getWonderImagePath(const QString& wonderName);
    int getCardId() const { return m_cardId; }

signals:
    void cardClicked(int cardId);

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private slots:
    void onButtonClicked();

private:
    Ui::CardWidget *ui;
    int m_cardId;
    bool m_isFaceUp;
    QString m_cardName;
    CardInfoPopup* m_popup;
    bool m_isWonder;
    QString adjustBrightness(const QString& color, double factor);
};