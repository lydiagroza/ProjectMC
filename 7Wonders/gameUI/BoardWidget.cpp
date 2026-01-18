#include "BoardWidget.h"
#include "ui_BoardWidget.h"
#include "SoundManager.h"

BoardWidget::BoardWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::BoardWidget)
{
    ui->setupUi(this);
    // Mărime minimă să încapă piramida
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

void BoardWidget::placeCard(int id, QString name, QString color, bool isFaceUp, int row, int col, int totalCardsInRow, QString cost, QString effect, QString unlocks)
{
    // Hide placeholder on first card placement
    if (ui->placeholderLabel && ui->placeholderLabel->isVisible()) {
        ui->placeholderLabel->setVisible(false);
    }

    // 1. Instanțiem widget-ul
    CardWidget* newCard = new CardWidget(id, this);
    newCard->setupCard(name, color, isFaceUp, cost, effect, unlocks);
    
    // Apply Scale
    int scaledWidth = static_cast<int>(BASE_CARD_WIDTH * m_scaleFactor);
    int scaledHeight = static_cast<int>(BASE_CARD_HEIGHT * m_scaleFactor);
    int scaledSpacing = static_cast<int>(BASE_H_SPACING * m_scaleFactor);
    int scaledOverlap = static_cast<int>(BASE_V_OVERLAP * m_scaleFactor);

    newCard->setFixedSize(scaledWidth, scaledHeight);

    // 2. Conectăm semnalul
    connect(newCard, &CardWidget::cardClicked, this, &BoardWidget::handleInternalClick);

    // 3. Matematică pentru poziționare
    // Centrul widget-ului părinte
    int centerX = this->width() / 2;
    int startY = 30 * m_scaleFactor;

    // Calculăm lățimea totală a rândului curent pentru centrare
    double rowTotalWidth = totalCardsInRow * scaledWidth + (totalCardsInRow - 1) * scaledSpacing;

    // Punctul de start X (stânga rândului)
    double startX = centerX - (rowTotalWidth / 2.0);

    // Coordonatele finale
    int posX = startX + col * (scaledWidth + scaledSpacing);
    int posY = startY + row * (scaledHeight - scaledOverlap);

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

        // Deselectăm vizual celelalte cărți
        for (auto c : m_activeCards) c->setSelected(false);

        // Selectăm cartea curentă
        senderCard->setSelected(true);

        // Anunțăm MainWindow
        emit cardClicked(cardId);
    }
}