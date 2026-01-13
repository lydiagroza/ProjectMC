#include "MainWindow.h"
#include "CardBase.h"
#include "Wonder.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>
#include <QStatusBar>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_game(new Game()), m_selectedCardId(-1), m_draftPhase(0)
{
    // 1. Configurare Fereastră
    this->setWindowTitle("7 Wonders Duel - Qt Edition");
    this->setStyleSheet(
        "QMainWindow { background-color: #2c3e50; }"
        "QLabel { font-family: 'Segoe UI'; }"
        "QStatusBar { color: white; }"
    );

    // 2. Widget Central și Layout Principal
    QWidget* centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // ZONA STÂNGA
    QFrame* leftZone = new QFrame();
    leftZone->setStyleSheet("background-color: transparent;");
    QVBoxLayout* leftLayout = new QVBoxLayout(leftZone);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    // A. Zona Oponent
    m_opponentZone = new QFrame();
    m_opponentZone->setFixedHeight(120); // 1. Increase height
    m_opponentZone->setStyleSheet("background-color: rgba(192, 57, 43, 0.4); border-bottom: 2px solid #c0392b; border-radius: 5px; padding: 5px;");
    QVBoxLayout* oppLay = new QVBoxLayout(m_opponentZone);

    m_opponentLabel = new QLabel("OPPONENT", m_opponentZone);
    m_opponentLabel->setStyleSheet("color: white; font-weight: bold; background: transparent; border: none;");
    m_opponentLabel->setAlignment(Qt::AlignHCenter);

    // 2. Add layout for opponent's wonders
    QHBoxLayout* opponentWondersLayout = new QHBoxLayout();
    opponentWondersLayout->setSpacing(5);

    oppLay->addWidget(m_opponentLabel);
    oppLay->addLayout(opponentWondersLayout); // Add the wonders layout to the zone

    // B. Zona Centrală (STACKED WIDGET)
    m_stack = new QStackedWidget();
    m_wonderSelection = new WonderSelectionWidget(this);
    m_stack->addWidget(m_wonderSelection);
    m_boardWidget = new BoardWidget(this);
    m_boardWidget->setStyleSheet("background-color: rgba(255, 255, 255, 0.05); border-radius: 10px;");
    m_stack->addWidget(m_boardWidget);

    // C. Zona Jucător
    m_playerZone = new QFrame();
    m_playerZone->setFixedHeight(140); // 3. Increase height
    m_playerZone->setStyleSheet("background-color: rgba(39, 174, 96, 0.4); border-top: 2px solid #27ae60; border-radius: 5px; padding: 5px;");
    QVBoxLayout* pLay = new QVBoxLayout(m_playerZone);

    m_playerLabel = new QLabel("PLAYER", m_playerZone);
    m_playerLabel->setStyleSheet("color: white; font-weight: bold; background: transparent; border: none;");
    m_playerLabel->setAlignment(Qt::AlignHCenter);

    // 4. Add layout for player's wonders
    QHBoxLayout* playerWondersLayout = new QHBoxLayout();
    playerWondersLayout->setSpacing(5);

    pLay->addWidget(m_playerLabel);
    pLay->addLayout(playerWondersLayout); // Add the wonders layout to the zone

    leftLayout->addWidget(m_opponentZone);
    leftLayout->addWidget(m_stack, 1);
    leftLayout->addWidget(m_playerZone);

    // ZONA DREAPTA
    QFrame* rightZone = new QFrame();
    rightZone->setFixedWidth(260);
    rightZone->setStyleSheet("background-color: #34495e; border-left: 3px solid #2980b9;");
    QVBoxLayout* rightLayout = new QVBoxLayout(rightZone);

    // --- REMOVE OLD PLACEHOLDERS AND ADD NEW WIDGETS ---

    // 1. Military Track
    m_militaryTrackWidget = new MilitaryTrackWidget(rightZone);

    // 2. Progress Tokens Area
    QLabel* lblTok = new QLabel("PROGRESS TOKENS", rightZone);
    lblTok->setStyleSheet("color: #2ecc71; font-weight: bold; font-size: 14px;");
    lblTok->setAlignment(Qt::AlignCenter);
    m_progressTokensLayout = new QVBoxLayout(); // This will hold the tokens

    // 3. Construct Button
    QPushButton* btnConstruct = new QPushButton("CONSTRUIESTE", rightZone);
    btnConstruct->setFixedHeight(50);
    btnConstruct->setStyleSheet("background-color: #f1c40f; color: black; font-weight: bold; font-size: 14px;");

    // Add widgets to the right layout
    rightLayout->addWidget(m_militaryTrackWidget);
    rightLayout->addStretch();
    rightLayout->addWidget(lblTok);
    rightLayout->addLayout(m_progressTokensLayout);
    rightLayout->addStretch();
    rightLayout->addWidget(btnConstruct);

    // FINALIZARE
    mainLayout->addWidget(leftZone, 1);
    mainLayout->addWidget(rightZone, 0);

    // Conectare Semnale
    connect(m_boardWidget, &BoardWidget::cardClicked, this, &MainWindow::onCardSelected);
    connect(btnConstruct, &QPushButton::clicked, this, &MainWindow::onConstructClicked);
    connect(m_wonderSelection, &WonderSelectionWidget::wonderChosen, this, &MainWindow::onWonderChosen);

    // Pornire Joc
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
    MainWindow::updateGameState();
}

void MainWindow::updateGameState()
{
    m_draftPhase = m_game->getDraftPhase();
    if (m_draftPhase > 0 && !m_game->getCurrentDraftSet().empty()) {
        m_stack->setCurrentIndex(0);
        startWonderDraft();
    } else {
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
        colors.push_back("#f39c12");
    }

    QString currentPlayerName = QString::fromStdString(m_game->getCurrentPlayer()->getName());
    this->statusBar()->showMessage("Este randul lui " + currentPlayerName + " sa aleaga o minune.", 5000);

    m_wonderSelection->setWonders(ids, names, colors);
}

void MainWindow::onWonderChosen(int wonderId)
{
    if (m_game->draftWonder(wonderId)) {
        updateGameState();
    } else {
        QMessageBox::warning(this, "Alegere Invalidă", "A apărut o eroare. Minunea selectată nu este validă.");
    }
}

void MainWindow::onCardSelected(int cardId)
{
    m_selectedCardId = cardId;
    qDebug() << "Carte selectata:" << cardId;
}

void MainWindow::onConstructClicked()
{
    if (m_selectedCardId == -1) {
        QMessageBox::warning(this, "Eroare", "Selecteaza o carte din piramida!");
        return;
    }
    QMessageBox::information(this, "Constructie", "Constructie initiata pentru cartea " + QString::number(m_selectedCardId));
}

void MainWindow::updatePlayerInventories()
{
    if (!m_game) return;

    const Player* player = m_game->getCurrentPlayer();
    const Player* opponent = m_game->getOpponent();

    if (!player || !opponent) return;

    // --- Update Player and Opponent Labels (existing code) ---
    QString playerText = QString("Jucator: %1 | Bani: %2").arg(QString::fromStdString(player->getName())).arg(player->getCoins());
    m_playerLabel->setText(playerText);

    QString opponentText = QString("Oponent: %1 | Bani: %2").arg(QString::fromStdString(opponent->getName())).arg(opponent->getCoins());
    m_opponentLabel->setText(opponentText);

    // --- NEW: Function to clear and draw wonders ---
    auto setupWonders = [&](const Player* p, QFrame* zone) {
        // Find the QHBoxLayout inside the zone's QVBoxLayout
        QVBoxLayout* mainZoneLayout = qobject_cast<QVBoxLayout*>(zone->layout());
        if (!mainZoneLayout || mainZoneLayout->count() < 2) return;
        QHBoxLayout* wondersLayout = qobject_cast<QHBoxLayout*>(mainZoneLayout->itemAt(1)->layout());
        if (!wondersLayout) return;

        // Clear previous wonder widgets
        QLayoutItem* item;
        while ((item = wondersLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }

        // Get wonders and create new widgets
        for (const auto& wonderPtr : p->getWonders()) {
            CardWidget* wonderWidget = new CardWidget(wonderPtr->getId(), zone);
            wonderWidget->setFixedSize(60, 80);
            
            QString name = QString::fromStdString(wonderPtr->getName());
            QString color = wonderPtr->getIsBuilt() ? "#f1c40f" : "#bdc3c7"; // Yellow if built, gray otherwise
            
            wonderWidget->setupCard(name, color, true); // Wonders are always face up
            wondersLayout->addWidget(wonderWidget);
        }
    };

    // --- NEW: Call the function for both players ---
    setupWonders(player, m_playerZone);
    setupWonders(opponent, m_opponentZone);

    // --- NEW: Update Military Track and Progress Tokens ---
    m_militaryTrackWidget->updatePawnPosition(m_game->getBoard().getMilitaryTrack().getPawnPosition());

    // Clear old token widgets
    QLayoutItem* item;
    while ((item = m_progressTokensLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Add tokens for both players
    auto addPlayerTokens = [&](const Player* p) {
        for (const auto& tokenPtr : p->getProgressTokens()) {
            QLabel* tokenLabel = new QLabel(QString::fromStdString(tokenPtr->getName()), this);
            tokenLabel->setStyleSheet("background-color: #2ecc71; color: black; padding: 5px; border-radius: 3px;");
            tokenLabel->setAlignment(Qt::AlignCenter);
            m_progressTokensLayout->addWidget(tokenLabel);
        }
    };

    addPlayerTokens(player);
    addPlayerTokens(opponent);
}

QString MainWindow::getColorHex(Color c) {
    switch (c) {
    case Color::Blue:   return "#2980b9";
    case Color::Red:    return "#c0392b";
    case Color::Green:  return "#27ae60";
    case Color::Yellow: return "#f1c40f";
    case Color::Brown:  return "#d35400";
    case Color::Gray:   return "#7f8c8d";
    case Color::Purple: return "#8e44ad";
    default: return "#95a5a6";
    }
}

void MainWindow::renderGame() {
    if (!m_boardWidget || !m_game) return;

    m_boardWidget->clearBoard();
    Board& boardData = m_game->getBoard();
    const auto& rows = boardData.getPyramid().getRows();

    for (int r = 0; r < rows.size(); ++r) {
        const auto& row = rows[r];
        int cardsInRow = row.size();
        for (int c = 0; c < cardsInRow; ++c) {
            const auto& node = row[c];

            if (!node || node->isPlayed()) continue;
            auto cardPtr = node->getCard();
            if (!cardPtr) continue;

            int id = cardPtr->getId();
            QString name = QString::fromStdString(cardPtr->getName());
            Color col = cardPtr->getColor();
            bool isFaceUp = (node->getFace() == Face::Up);

            m_boardWidget->placeCard(id, name, getColorHex(col), isFaceUp, r, c, cardsInRow);
        }
    }
}           