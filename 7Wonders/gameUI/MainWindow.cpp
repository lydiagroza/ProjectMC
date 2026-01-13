#include "MainWindow.h"
#include "CardBase.h"
#include "Wonder.h"
#include "CardWidget.h"
#include "BoardWidget.h"
#include "WonderSelectionWidget.h"
#include "MilitaryTrackWidget.h"
#include "Game.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>
#include <QStatusBar>
#include <QGraphicsDropShadowEffect>
#include <QFrame>
#include <QStackedWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_game(new Game()), m_selectedCardId(-1), m_draftPhase(0)
{
    this->setWindowTitle("⚔️ 7 Wonders Duel - Age of Empires ⚔️");

    this->setStyleSheet(
        "QMainWindow { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #2C1810, stop:0.5 #3E2723, stop:1 #2C1810); "
        "}"
        "QLabel { "
        "  font-family: 'Times New Roman', serif; "
        "  color: #F5E6D3; "
        "}"
        "QStatusBar { "
        "  background-color: #3E2723; "
        "  color: #DAA520; "
        "  border-top: 2px solid #8B4513; "
        "  font-family: 'Times New Roman'; "
        "  font-weight: bold; "
        "}"
    );

    QWidget* centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    QFrame* leftZone = new QFrame();
    leftZone->setStyleSheet("background-color: transparent;");
    QVBoxLayout* leftLayout = new QVBoxLayout(leftZone);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    m_opponentZone = new QFrame();
    m_opponentZone->setFixedHeight(130);
    m_opponentZone->setStyleSheet(
        "QFrame { "
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "    stop:0 #8B0000, stop:1 #5C0000); "
        "  border: 3px solid #8B4513; "
        "  border-radius: 10px; "
        "  padding: 8px; "
        "}"
    );
    QVBoxLayout* oppLay = new QVBoxLayout(m_opponentZone);

    m_opponentLabel = new QLabel("⚔️ OPPONENT EMPIRE ⚔️", m_opponentZone);
    m_opponentLabel->setStyleSheet(
        "color: #FFD700; "
        "font-weight: bold; "
        "font-size: 14px; "
        "background: transparent; "
        "border: none; "
        "font-family: 'Times New Roman', serif;"
    );
    m_opponentLabel->setAlignment(Qt::AlignHCenter);

    QGraphicsDropShadowEffect* oppShadow = new QGraphicsDropShadowEffect();
    oppShadow->setBlurRadius(8);
    oppShadow->setColor(QColor(0, 0, 0, 180));
    oppShadow->setOffset(2, 2);
    m_opponentLabel->setGraphicsEffect(oppShadow);

    QHBoxLayout* opponentWondersLayout = new QHBoxLayout();
    opponentWondersLayout->setSpacing(5);

    oppLay->addWidget(m_opponentLabel);
    oppLay->addLayout(opponentWondersLayout);

    m_stack = new QStackedWidget();
    m_wonderSelection = new WonderSelectionWidget(this);
    m_stack->addWidget(m_wonderSelection);
    m_boardWidget = new BoardWidget(this);
    m_boardWidget->setStyleSheet(
        "background: qradialgradient(cx:0.5, cy:0.5, radius:0.8, "
        "  fx:0.5, fy:0.5, stop:0 #4A3728, stop:1 #2C1810); "
        "border: 4px solid #8B4513; "
        "border-radius: 15px;"
    );
    m_stack->addWidget(m_boardWidget);

    m_playerZone = new QFrame();
    m_playerZone->setFixedHeight(150);
    m_playerZone->setStyleSheet(
        "QFrame { "
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "    stop:0 #1565C0, stop:1 #0D47A1); "
        "  border: 3px solid #8B4513; "
        "  border-radius: 10px; "
        "  padding: 8px; "
        "}"
    );
    QVBoxLayout* pLay = new QVBoxLayout(m_playerZone);

    m_playerLabel = new QLabel("🏛️ YOUR CIVILIZATION 🏛️", m_playerZone);
    m_playerLabel->setStyleSheet(
        "color: #FFD700; "
        "font-weight: bold; "
        "font-size: 14px; "
        "background: transparent; "
        "border: none; "
        "font-family: 'Times New Roman', serif;"
    );
    m_playerLabel->setAlignment(Qt::AlignHCenter);

    QGraphicsDropShadowEffect* playerShadow = new QGraphicsDropShadowEffect();
    playerShadow->setBlurRadius(8);
    playerShadow->setColor(QColor(0, 0, 0, 180));
    playerShadow->setOffset(2, 2);
    m_playerLabel->setGraphicsEffect(playerShadow);

    QHBoxLayout* playerWondersLayout = new QHBoxLayout();
    playerWondersLayout->setSpacing(5);

    pLay->addWidget(m_playerLabel);
    pLay->addLayout(playerWondersLayout);

    leftLayout->addWidget(m_opponentZone);
    leftLayout->addWidget(m_stack, 1);
    leftLayout->addWidget(m_playerZone);

    QFrame* rightZone = new QFrame();
    rightZone->setFixedWidth(280);
    rightZone->setStyleSheet(
        "QFrame { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #3E2723, stop:0.5 #4E342E, stop:1 #3E2723); "
        "  border-left: 4px solid #8B4513; "
        "}"
    );
    QVBoxLayout* rightLayout = new QVBoxLayout(rightZone);

    QLabel* panelTitle = new QLabel("📜 GAME STATUS 📜", rightZone);
    panelTitle->setStyleSheet(
        "color: #DAA520; "
        "font-weight: bold; "
        "font-size: 16px; "
        "background: transparent; "
        "padding: 10px; "
        "font-family: 'Times New Roman', serif;"
    );
    panelTitle->setAlignment(Qt::AlignCenter);

    QGraphicsDropShadowEffect* titleShadow = new QGraphicsDropShadowEffect();
    titleShadow->setBlurRadius(6);
    titleShadow->setColor(QColor(0, 0, 0, 200));
    titleShadow->setOffset(2, 2);
    panelTitle->setGraphicsEffect(titleShadow);

    m_militaryTrackWidget = new MilitaryTrackWidget(rightZone);

    QLabel* lblTok = new QLabel("⚡ DIVINE BLESSINGS ⚡", rightZone);
    lblTok->setStyleSheet(
        "color: #FFD700; "
        "font-weight: bold; "
        "font-size: 13px; "
        "padding: 5px; "
        "font-family: 'Times New Roman';"
    );
    lblTok->setAlignment(Qt::AlignCenter);

    QGraphicsDropShadowEffect* tokShadow = new QGraphicsDropShadowEffect();
    tokShadow->setBlurRadius(6);
    tokShadow->setColor(QColor(0, 0, 0, 200));
    tokShadow->setOffset(1, 1);
    lblTok->setGraphicsEffect(tokShadow);

    m_progressTokensLayout = new QVBoxLayout();

    QPushButton* btnConstruct = new QPushButton("⚒️ CONSTRUIESTE ⚒️", rightZone);
    btnConstruct->setFixedHeight(60);
    btnConstruct->setCursor(Qt::PointingHandCursor);
    btnConstruct->setStyleSheet(
        "QPushButton { "
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "    stop:0 #B8860B, stop:0.5 #DAA520, stop:1 #B8860B); "
        "  color: #2C1810; "
        "  border: 3px solid #8B4513; "
        "  border-radius: 10px; "
        "  font-weight: bold; "
        "  font-size: 13px; "
        "  font-family: 'Times New Roman', serif; "
        "}"
        "QPushButton:hover { "
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "    stop:0 #DAA520, stop:0.5 #FFD700, stop:1 #DAA520); "
        "  border: 3px solid #FFD700; "
        "}"
        "QPushButton:pressed { "
        "  background: #8B6914; "
        "}"
    );

    rightLayout->addWidget(panelTitle);
    rightLayout->addWidget(m_militaryTrackWidget);
    rightLayout->addStretch();
    rightLayout->addWidget(lblTok);
    rightLayout->addLayout(m_progressTokensLayout);
    rightLayout->addStretch();
    rightLayout->addWidget(btnConstruct);

    mainLayout->addWidget(leftZone, 1);
    mainLayout->addWidget(rightZone, 0);

    connect(m_boardWidget, &BoardWidget::cardClicked, this, &MainWindow::onCardSelected);
    connect(btnConstruct, &QPushButton::clicked, this, &MainWindow::onConstructClicked);
    connect(m_wonderSelection, &WonderSelectionWidget::wonderChosen, this, &MainWindow::onWonderChosen);

    this->statusBar()->showMessage("⚔️ Prepare for glory! Choose your wonders... ⚔️");

    m_stack->setCurrentIndex(0);
    QTimer::singleShot(100, this, &MainWindow::startGame);
}

MainWindow::~MainWindow()
{
    delete m_game;
}

void MainWindow::startGame()
{
    m_game->initialize();
    updateGameState();
}

void MainWindow::updateGameState()
{
    m_draftPhase = m_game->getDraftPhase();
    if (m_draftPhase > 0 && !m_game->getCurrentDraftSet().empty()) {
        m_stack->setCurrentIndex(0);
        startWonderDraft();
    }
    else {
        m_stack->setCurrentIndex(1);
        renderGame();
    }
    updatePlayerInventories();
}

void MainWindow::startWonderDraft()
{
    const auto& availableWonders = m_game->getCurrentDraftSet();
    if (availableWonders.empty()) {
        updateGameState();
        return;
    }

    std::vector<int> ids;
    std::vector<QString> names;
    std::vector<QString> colors;

    for (const auto& wonderPtr : availableWonders) {
        ids.push_back(wonderPtr->getId());
        names.push_back(QString::fromStdString(wonderPtr->getName()));
        colors.push_back("#DAA520");
    }

    QString currentPlayerName = QString::fromStdString(m_game->getCurrentPlayer()->getName());
    this->statusBar()->showMessage("⚔️ " + currentPlayerName + " must choose a wonder! ⚔️", 5000);

    m_wonderSelection->setWonders(ids, names, colors);
}

void MainWindow::onWonderChosen(int wonderId)
{
    if (m_game->draftWonder(wonderId)) {
        updateGameState();
    }
    else {
        QMessageBox::warning(this, "Invalid Choice", "An error occurred. The selected wonder is not valid.");
    }
}

void MainWindow::onCardSelected(int cardId)
{
    m_selectedCardId = cardId;
    qDebug() << "Card selected:" << cardId;
}

void MainWindow::onConstructClicked()
{
    if (m_selectedCardId == -1) {
        QMessageBox::warning(this, "Error", "Select a card from the pyramid!");
        return;
    }
    QMessageBox::information(this, "Construction", "Construction initiated for card " + QString::number(m_selectedCardId));
}

void MainWindow::updatePlayerInventories()
{
    if (!m_game) return;

    const Player* player = m_game->getCurrentPlayer();
    const Player* opponent = m_game->getOpponent();

    if (!player || !opponent) return;

    QString playerText = QString("🏛️ %1 | 💰 %2 Denarii")
        .arg(QString::fromStdString(player->getName()))
        .arg(player->getCoins());
    m_playerLabel->setText(playerText);

    QString opponentText = QString("⚔️ %1 | 💰 %2 Denarii")
        .arg(QString::fromStdString(opponent->getName()))
        .arg(opponent->getCoins());
    m_opponentLabel->setText(opponentText);

    // ✅ FĂRĂ punct și virgulă după lambda
    auto setupWonders = [this](const Player* p, QFrame* zone) {
        QVBoxLayout* mainZoneLayout = qobject_cast<QVBoxLayout*>(zone->layout());
        if (!mainZoneLayout || mainZoneLayout->count() < 2) return;
        QHBoxLayout* wondersLayout = qobject_cast<QHBoxLayout*>(mainZoneLayout->itemAt(1)->layout());
        if (!wondersLayout) return;

        QLayoutItem* item;
        while ((item = wondersLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }

        for (const auto& wonderPtr : p->getWonders()) {
            CardWidget* wonderWidget = new CardWidget(wonderPtr->getId(), zone);
            wonderWidget->setFixedSize(70, 95);

            QString name = QString::fromStdString(wonderPtr->getName());
            QString color = wonderPtr->getIsBuilt() ? "#DAA520" : "#6D4C41";

            wonderWidget->setupCard(name, color, true);
            wondersLayout->addWidget(wonderWidget);
        }
        }; // ✅ Punct și virgulă DOAR aici

    setupWonders(player, m_playerZone);
    setupWonders(opponent, m_opponentZone);

    m_militaryTrackWidget->updatePawnPosition(m_game->getBoard().getMilitaryTrack().getPawnPosition());

    QLayoutItem* item;
    while ((item = m_progressTokensLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // ✅ FĂRĂ punct și virgulă după lambda
    auto addPlayerTokens = [this](const Player* p) {
        for (const auto& tokenPtr : p->getProgressTokens()) {
            QLabel* tokenLabel = new QLabel("⚡ " + QString::fromStdString(tokenPtr->getName()), this);
            tokenLabel->setStyleSheet(
                "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
                "  stop:0 #FFD700, stop:0.5 #FFF9C4, stop:1 #FFD700); "
                "color: #3E2723; "
                "padding: 8px; "
                "border: 2px solid #8B4513; "
                "border-radius: 6px; "
                "font-weight: bold; "
                "font-family: 'Times New Roman';"
            );
            tokenLabel->setAlignment(Qt::AlignCenter);

            QGraphicsDropShadowEffect* tokenShadow = new QGraphicsDropShadowEffect();
            tokenShadow->setBlurRadius(4);
            tokenShadow->setColor(QColor(0, 0, 0, 150));
            tokenShadow->setOffset(2, 2);
            tokenLabel->setGraphicsEffect(tokenShadow);

            m_progressTokensLayout->addWidget(tokenLabel);
        }
        }; // ✅ Punct și virgulă DOAR aici

    addPlayerTokens(player);
    addPlayerTokens(opponent);
}

QString MainWindow::getColorHex(Color c) {
    switch (c) {
    case Color::Blue:   return "#1565C0";
    case Color::Red:    return "#B71C1C";
    case Color::Green:  return "#2E7D32";
    case Color::Yellow: return "#F57F17";
    case Color::Brown:  return "#5D4037";
    case Color::Gray:   return "#616161";
    case Color::Purple: return "#6A1B9A";
    default: return "#8B7355";
    }
}

void MainWindow::renderGame() {
    if (!m_boardWidget || !m_game) return;

    m_boardWidget->clearBoard();
    Board& boardData = m_game->getBoard();
    const auto& rows = boardData.getPyramid().getRows();

    for (size_t r = 0; r < rows.size(); ++r) {
        const auto& row = rows[r];
        int cardsInRow = static_cast<int>(row.size());

        for (size_t c = 0; c < row.size(); ++c) {
            const auto& node = row[c];

            if (!node || node->isPlayed()) continue;
            auto cardPtr = node->getCard();
            if (!cardPtr) continue;

            int id = cardPtr->getId();
            QString name = QString::fromStdString(cardPtr->getName());
            Color col = cardPtr->getColor();
            bool isFaceUp = (node->getFace() == Face::Up);

            m_boardWidget->placeCard(id, name, getColorHex(col), isFaceUp,
                static_cast<int>(r), static_cast<int>(c), cardsInRow);
        }
    }
}