#include "BoardWidget.h"
#include "ui_BoardWidget.h"
#include "SoundManager.h"

BoardWidget::BoardWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::BoardWidget)
{
    ui->setupUi(this);
    this->setMinimumSize(800, 600);
}

BoardWidget::~BoardWidget()
{
    delete ui;
}

void BoardWidget::clearBoard()
{
    for (auto card : m_activeCards) {
        card->deleteLater();
    }
    m_activeCards.clear();
    
    if(ui->placeholderLabel) ui->placeholderLabel->setVisible(true);
}

void BoardWidget::placeCard(int id, QString name, QString color, bool isFaceUp, int row, int col, int totalCardsInRow, int age, QString cost, QString effect, QString unlocks)
{
    if (ui->placeholderLabel && ui->placeholderLabel->isVisible()) {
        ui->placeholderLabel->setVisible(false);
    }

    CardWidget* newCard = new CardWidget(id, this);
    newCard->setupCard(name, color, isFaceUp, cost, effect, unlocks);
    
    int scaledWidth = static_cast<int>(BASE_CARD_WIDTH * m_scaleFactor);
    int scaledHeight = static_cast<int>(BASE_CARD_HEIGHT * m_scaleFactor);
    int scaledSpacing = static_cast<int>(BASE_H_SPACING * m_scaleFactor);
    int scaledOverlap = static_cast<int>(BASE_V_OVERLAP * m_scaleFactor);

    newCard->setFixedSize(scaledWidth, scaledHeight);

    connect(newCard, &CardWidget::cardClicked, this, &BoardWidget::handleInternalClick);

    int centerX = this->width() / 2;
    int startY = 30 * m_scaleFactor;

    // coord 
    int posX;
    int posY = startY + row * (scaledHeight - scaledOverlap);

    if (age == 3 && totalCardsInRow == 2) {
        if (row == 3) {
           
            double rowTotalWidth_above = 4 * scaledWidth + (4 - 1) * scaledSpacing;
            double startX_above = centerX - (rowTotalWidth_above / 2.0);

            if (col == 0) { 
                posX = startX_above + 0.5 * (scaledWidth + scaledSpacing);
            } else { 
                posX = startX_above + 2.5 * (scaledWidth + scaledSpacing);
            }
        } else if (row == 6) {
           
            double rowTotalWidth_above = 3 * scaledWidth + (3 - 1) * scaledSpacing;
            double startX_above = centerX - (rowTotalWidth_above / 2.0);
            if (col == 0) {
                posX = startX_above + 0.5 * (scaledWidth + scaledSpacing);
            } else { 
                posX = startX_above + 1.5 * (scaledWidth + scaledSpacing);
            }
        }
        else {
            double rowTotalWidth = totalCardsInRow * scaledWidth + (totalCardsInRow - 1) * scaledSpacing;
            double startX = centerX - (rowTotalWidth / 2.0);
            posX = startX + col * (scaledWidth + scaledSpacing);
        }
    } else {
        double rowTotalWidth = totalCardsInRow * scaledWidth + (totalCardsInRow - 1) * scaledSpacing;
        double startX = centerX - (rowTotalWidth / 2.0);
        posX = startX + col * (scaledWidth + scaledSpacing);
    }

    newCard->move(posX, posY);
    newCard->show();

    m_activeCards.append(newCard);
}

void BoardWidget::handleInternalClick(int cardId)
{
    CardWidget* senderCard = nullptr;
    for (auto card : m_activeCards) {
        if (card->getCardId() == cardId) {
            senderCard = card;
            break;
        }
    }

    if (senderCard) {
        SoundManager::instance().playClick();
        for (auto c : m_activeCards) c->setSelected(false);
        senderCard->setSelected(true);
        emit cardClicked(cardId);
    }
}