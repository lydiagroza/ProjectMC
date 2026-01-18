#include "CardInfoPopup.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>

CardInfoPopup::CardInfoPopup(QWidget* parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::ToolTip)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    m_titleLabel = new QLabel(this);
    m_descriptionLabel = new QLabel(this);
    m_cardPreview = new QLabel(this);

    // --- Styling ---
    m_titleLabel->setStyleSheet(
        "QLabel { "
        "  color: #E0E0E0; "
        "  font-size: 16px; "
        "  font-weight: bold; "
        "  padding: 5px; "
        "}"
    );

    m_descriptionLabel->setStyleSheet(
        "QLabel { "
        "  color: #BDBDBD; "
        "  font-size: 12px; "
        "  padding: 5px; "
        "}"
    );
    m_descriptionLabel->setWordWrap(true);

    m_cardPreview->setScaledContents(true);
    m_cardPreview->setFixedSize(150, 210); // Slightly larger preview

    // --- Layout ---
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(5);
    layout->addWidget(m_cardPreview, 0, Qt::AlignHCenter);
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_descriptionLabel);

    setLayout(layout);
    setFixedSize(220, 320); // Adjusted size
}

void CardInfoPopup::setCardData(const QString& title, const QString& description, const QPixmap& cardImage)
{
    m_titleLabel->setText(title);
    m_descriptionLabel->setText(description);
    m_cardPreview->setPixmap(cardImage);
}

void CardInfoPopup::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Semi-transparent background with rounded corners
    painter.setBrush(QColor(30, 30, 30, 220));
    painter.setPen(QPen(QColor(80, 80, 80), 2));
    painter.drawRoundedRect(this->rect(), 10, 10);
}
