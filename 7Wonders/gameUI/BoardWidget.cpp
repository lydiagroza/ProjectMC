#include "BoardWidget.h"
#include "ui_BoardWidget.h"

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
}

void BoardWidget::placeCard(int id, QString name, QString color, bool isFaceUp, int row, int col, int totalCardsInRow, QString cost, QString effect)
{
    // 1. Instanțiem widget-ul
    CardWidget* newCard = new CardWidget(id, this);
    newCard->setupCard(name, color, isFaceUp, cost, effect);

    // 2. Conectăm semnalul
    connect(newCard, &CardWidget::cardClicked, this, &BoardWidget::handleInternalClick);

    // 3. Matematică pentru poziționare
    // Centrul widget-ului părinte
    int centerX = this->width() / 2;
    int startY = 30;

    // Calculăm lățimea totală a rândului curent pentru centrare
    double rowTotalWidth = totalCardsInRow * CARD_WIDTH + (totalCardsInRow - 1) * H_SPACING;

    // Punctul de start X (stânga rândului)
    double startX = centerX - (rowTotalWidth / 2.0);

    // Coordonatele finale
    int posX = startX + col * (CARD_WIDTH + H_SPACING);
    int posY = startY + row * (CARD_HEIGHT - V_OVERLAP);

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
        // Deselectăm vizual celelalte cărți
        for (auto c : m_activeCards) c->setSelected(false);

        // Selectăm cartea curentă
        senderCard->setSelected(true);

        // Anunțăm MainWindow
        emit cardClicked(cardId);
    }
}