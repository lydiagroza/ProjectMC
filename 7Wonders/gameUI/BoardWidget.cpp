#include "BoardWidget.h"
#include "ui_BoardWidget.h"
#include "SoundManager.h"

BoardWidget::BoardWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::BoardWidget)
{
    ui->setupUi(this);
    // Minimum size to fit the pyramid
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
    
    // Show placeholder if needed, though usually immediately repopulated
    if(ui->placeholderLabel) ui->placeholderLabel->setVisible(true);
}

void BoardWidget::placeCard(int id, QString name, QString color, bool isFaceUp, int row, int col, int totalCardsInRow, int age, QString cost, QString effect, QString unlocks)
{
    // Hide placeholder on first card placement
    if (ui->placeholderLabel && ui->placeholderLabel->isVisible()) {
        ui->placeholderLabel->setVisible(false);
    }

    // 1. Instantiate the widget
    CardWidget* newCard = new CardWidget(id, this);
    newCard->setupCard(name, color, isFaceUp, cost, effect, unlocks);
    
    // Apply Scale
    int scaledWidth = static_cast<int>(BASE_CARD_WIDTH * m_scaleFactor);
    int scaledHeight = static_cast<int>(BASE_CARD_HEIGHT * m_scaleFactor);
    int scaledSpacing = static_cast<int>(BASE_H_SPACING * m_scaleFactor);
    int scaledOverlap = static_cast<int>(BASE_V_OVERLAP * m_scaleFactor);

    newCard->setFixedSize(scaledWidth, scaledHeight);

    // 2. Connect the signal
    connect(newCard, &CardWidget::cardClicked, this, &BoardWidget::handleInternalClick);

    // 3. Mathematics for positioning
    // Center of the parent widget
    int centerX = this->width() / 2;
    int startY = 30 * m_scaleFactor;

    // Final coordinates
    int posX;
    int posY = startY + row * (scaledHeight - scaledOverlap);

    if (age == 3 && totalCardsInRow == 2) {
        if (row == 3) {
            // Age III: row 3 (2 cards), located between two 4-card rows.
            // Position relative to the row above (row 2, 4 cards).
            double rowTotalWidth_above = 4 * scaledWidth + (4 - 1) * scaledSpacing;
            double startX_above = centerX - (rowTotalWidth_above / 2.0);

            if (col == 0) { // First card of the row
                // Place between the first and second cards of the row above.
                posX = startX_above + 0.5 * (scaledWidth + scaledSpacing);
            } else { // Second card (col == 1)
                // Place between the third and fourth cards of the row above.
                posX = startX_above + 2.5 * (scaledWidth + scaledSpacing);
            }
        } else if (row == 6) {
             // Age III: row 6 (2 cards), located below a 3-card row.
             // Position relative to the row above (row 5, 3 cards).
            double rowTotalWidth_above = 3 * scaledWidth + (3 - 1) * scaledSpacing;
            double startX_above = centerX - (rowTotalWidth_above / 2.0);
            if (col == 0) {
                // Place between the first and second cards of the row above.
                posX = startX_above + 0.5 * (scaledWidth + scaledSpacing);
            } else { // Second card (col == 1)
                // Place between the second and third cards of the row above.
                posX = startX_above + 1.5 * (scaledWidth + scaledSpacing);
            }
        }
        else {
            // Default centering for any other 2-card rows (e.g., row 0 in Age III).
            double rowTotalWidth = totalCardsInRow * scaledWidth + (totalCardsInRow - 1) * scaledSpacing;
            double startX = centerX - (rowTotalWidth / 2.0);
            posX = startX + col * (scaledWidth + scaledSpacing);
        }
    } else {
        // Default centering for all other rows.
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

        // Visually deselect other cards
        for (auto c : m_activeCards) c->setSelected(false);

        // Select the current card
        senderCard->setSelected(true);

        // Notify MainWindow
        emit cardClicked(cardId);
    }
}