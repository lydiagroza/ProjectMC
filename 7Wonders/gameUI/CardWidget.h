#pragma once
#include <QWidget>
#include <QGraphicsDropShadowEffect>

namespace Ui {
class CardWidget;
}

class CardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CardWidget(int cardId, QWidget* parent = nullptr);
    ~CardWidget();

    void setupCard(const QString& name, const QString& colorCode, bool isFaceUp, const QString& cost = "", const QString& effect = "");
    void setSelected(bool selected);
    void setImage(const QString& imagePath); // New method to set the image
    static QString getWonderImagePath(const QString& wonderName); // Helper to find image path
    int getCardId() const { return m_cardId; }

signals:
    void clicked();

private slots:
    void onButtonClicked();

private:
    Ui::CardWidget *ui;
    int m_cardId;
    bool m_isFaceUp;
    QString adjustBrightness(const QString& color, double factor);
};