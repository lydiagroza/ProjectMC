#pragma once

#include <QWidget>
#include <QPixmap>

class QLabel;
class QVBoxLayout;

class CardInfoPopup : public QWidget
{
    Q_OBJECT

public:
    explicit CardInfoPopup(QWidget* parent = nullptr);
    void setCardData(const QString& title, const QString& description, const QPixmap& cardImage);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QLabel* m_titleLabel;
    QLabel* m_descriptionLabel;
    QLabel* m_cardPreview;
};
