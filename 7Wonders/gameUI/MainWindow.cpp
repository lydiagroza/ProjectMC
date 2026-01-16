#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "CardBase.h"
#include "Wonder.h"
#include "CardWidget.h"
#include "BoardWidget.h"
#include "WonderSelectionWidget.h"
#include "NameSelectionWidget.h"
#include "WonderChoiceDialog.h"
#include "AI_Player.h"
#include "MilitaryTrackWidget.h"
#include "SplashScreen.h"
#include "PlayerDashboardWidget.h"
#include "Game.h"
#include "GameConstants.h"
#include "DiscardedCardsDialog.h"

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
#include <QInputDialog>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_game(new Game())
    , m_selectedCardId(-1)
    , m_draftPhase(0)
{
    ui->setupUi(this);

    qDebug() << "PlayerDashboard Ptr:" << ui->playerDashboard << " Y:" << ui->playerDashboard->y();
    qDebug() << "OpponentDashboard Ptr:" << ui->opponentDashboard << " Y:" << ui->opponentDashboard->y();

    // Apply Shadows for Right Panel items
    QGraphicsDropShadowEffect* titleShadow = new QGraphicsDropShadowEffect();
    titleShadow->setBlurRadius(6);
    titleShadow->setColor(QColor(0, 0, 0, 200));
    titleShadow->setOffset(2, 2);
    ui->panelTitle->setGraphicsEffect(titleShadow);

    QGraphicsDropShadowEffect* tokShadow = new QGraphicsDropShadowEffect();
    tokShadow->setBlurRadius(6);
    tokShadow->setColor(QColor(0, 0, 0, 200));
    tokShadow->setOffset(1, 1);
    ui->lblTok->setGraphicsEffect(tokShadow);

    // Apply style to BoardWidget
    ui->boardWidgetPage->setStyleSheet(
        "background: transparent; "
        "border: 4px solid #8B4513; "
        "border-radius: 15px;"
    );

    // Initialize Dashboard Themes
    ui->opponentDashboard->setTheme(true);
    ui->playerDashboard->setTheme(false);

    // Connect Signals
    connect(ui->boardWidgetPage, &BoardWidget::cardClicked, this, &MainWindow::onCardSelected);
    
    // Action Buttons
    connect(ui->btnBuild, &QPushButton::clicked, this, &MainWindow::onBuildClicked);
    connect(ui->btnDiscard, &QPushButton::clicked, this, &MainWindow::onDiscardClicked);
    connect(ui->btnWonder, &QPushButton::clicked, this, &MainWindow::onWonderClicked);
    
    connect(ui->wonderSelectionPage, &WonderSelectionWidget::wonderChosen, this, &MainWindow::onWonderChosen);
    connect(ui->splashScreenPage, &SplashScreen::gameModeSelected, this, &MainWindow::onSplashFinished);
    connect(ui->nameSelectionPage, &NameSelectionWidget::namesConfirmed, this, &MainWindow::onNamesConfirmed);
    connect(ui->btnDiscardedCards, &QPushButton::clicked, this, &MainWindow::onDiscardedCardsClicked);

    ui->statusbar->showMessage("⚔️ Welcome to 7 Wonders Duel! ⚔️");

    // Hide zones initially
    ui->opponentDashboard->setVisible(false);
    ui->playerDashboard->setVisible(false);
    ui->rightZone->setVisible(false);
    ui->turnContainer->setVisible(false);

    // Initial button state
    ui->btnBuild->setEnabled(false);
    ui->btnDiscard->setEnabled(false);
    ui->btnWonder->setEnabled(false);

    // Start at Splash Screen
    ui->stack->setCurrentIndex(0);

    // Ensure the window fills the screen for the best game experience
    this->showMaximized();
}

MainWindow::~MainWindow()
{
    delete m_game;
    delete ui;
}

void MainWindow::onSplashFinished(SplashScreen::GameMode mode)
{
    m_gameMode = mode;
    if (mode == SplashScreen::AvAI) {
        onNamesConfirmed("AI Player 1", "AI Player 2", 1); // Default Medium for quick start
    } else {
        // Player 1 is always human in PvA and PvP
        // Player 2 is only human in PvP
        ui->nameSelectionPage->setMode(true, mode == SplashScreen::PvP);
        ui->stack->setCurrentWidget(ui->nameSelectionPage);
        ui->statusbar->showMessage("⚔️ Who dares to challenge the ducks? ⚔️");
    }
}

void MainWindow::onNamesConfirmed(const QString& p1, const QString& p2, int difficulty)
{
    if (m_game) {
        m_game->getPlayer1().setName(p1.toStdString());
        m_game->getPlayer2().setName(p2.toStdString());
        
        AI_Difficulty diff = static_cast<AI_Difficulty>(difficulty);
        m_game->setPlayerTypes(m_gameMode == SplashScreen::AvAI, m_gameMode != SplashScreen::PvP, diff);
    }

    ui->opponentDashboard->setVisible(true);
    ui->playerDashboard->setVisible(true);
    ui->rightZone->setVisible(true);
    ui->turnContainer->setVisible(true);

    ui->statusbar->showMessage("⚔️ Initializing the ancient world of ducks... ⚔️");
    startGame();
}

void MainWindow::processAITurn()
{
    AI_Player* ai = dynamic_cast<AI_Player*>(m_game->getCurrentPlayer());
    if (!ai || m_game->isGameOver()) return;

    ui->statusbar->showMessage("🤖 " + QString::fromStdString(ai->getName()) + " is thinking...", 1000);

    ai->makeDecision(m_game->getBoard(), *m_game->getOpponent(), m_game->getCurrentAge());

    // After AI move, refresh game state
    if (!ai->hasExtraTurn()) {
        m_game->switchTurn();
    } else {
        ai->setHasExtraTurn(false);
        ui->statusbar->showMessage("🤖 " + QString::fromStdString(ai->getName()) + " gets an extra turn!", 2000);
    }
    
    updateGameState();
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
        ui->stack->setCurrentWidget(ui->wonderSelectionPage);
        startWonderDraft();
    }
    else {
        ui->stack->setCurrentWidget(ui->boardWidgetPage);
        renderGame();
    }
    updatePlayerInventories();
    updateTurnIndicator();

    // Check for INSTANT WIN (Military or Science)
    if (m_game->checkForInstantWin()) {
        std::optional<Player> winner = m_game->determinateWinner();
        if (winner) {
            QString winnerName = QString::fromStdString(winner->getName());
            QMessageBox::information(this, "Victory!", winnerName + " has won the game due to military or scientific supremacy!");
        }
        else {
            // This case should ideally not happen if checkForInstantWin is true, but as a fallback:
            QMessageBox::information(this, "Game Over", "The game has ended!");
        }
        
        // Disable all game interactions
        ui->btnBuild->setEnabled(false);
        ui->btnDiscard->setEnabled(false);
        ui->btnWonder->setEnabled(false);
        ui->boardWidgetPage->setEnabled(false);
        return; // Stop further processing
    }
    
    // Check for end of age
    if (m_game->isEndOfAge()) {
        QMessageBox::information(this, "Age Complete", "The current age has ended!");
        m_game->startNextAge();
        if (m_game->isGameOver()) {
             QMessageBox::information(this, "Game Over", "The game has ended!");
             return;
        }
        renderGame();
    }

    // Reset selection state
    m_selectedCardId = -1;
    ui->lblCost->setText("Cost: -");
    ui->btnBuild->setEnabled(false);
    ui->btnDiscard->setEnabled(false);
    ui->btnWonder->setEnabled(false);
    
    // Check for game over
    if (m_game->isGameOver()) {
        QMessageBox::information(this, "Game Over", "The game has ended!");
        return;
    }

    // Check for AI Turn (only if NOT in draft phase)
    if (m_draftPhase == 0 || m_game->getCurrentDraftSet().empty()) {
        AI_Player* ai = dynamic_cast<AI_Player*>(m_game->getCurrentPlayer());
        if (ai) {
            QTimer::singleShot(800, this, &MainWindow::processAITurn);
        }
    }
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
    std::vector<QString> costs;
    std::vector<QString> effects;

    auto formatCost = [](const std::map<Resource, uint8_t>& costMap) -> QString {
        QStringList parts;
        for (auto const& [res, count] : costMap) {
             QString icon;
             switch(res) {
                 case Resource::Wood: icon = "wood.png"; break;
                 case Resource::Clay: icon = "clay.png"; break;
                 case Resource::Stone: icon = "stone.png"; break;
                 case Resource::Glass: icon = "glass.png"; break;
                 case Resource::Papyrus: icon = "papyrus.png"; break;
                 case Resource::Coin: icon = "coin.png"; break;
             }
             if (!icon.isEmpty()) {
                parts << QString("%1 x <img src=':/resources/UI/%2' height='14'>").arg(count).arg(icon);
             }
        }
        return parts.join("<br>");
    };

    for (const auto& wonderPtr : availableWonders) {
        ids.push_back(wonderPtr->getId());
        names.push_back(QString::fromStdString(wonderPtr->getName()));
        colors.push_back("#DAA520");
        costs.push_back(formatCost(wonderPtr->getCost()));
        effects.push_back(QString::fromStdString(wonderPtr->getEffectDescription()));
    }

    QString currentPlayerName = QString::fromStdString(m_game->getCurrentPlayer()->getName());
    ui->statusbar->showMessage("⚔️ " + currentPlayerName + " must choose a wonder! ⚔️", 5000);

    // AI Check
    AI_Player* ai = dynamic_cast<AI_Player*>(m_game->getCurrentPlayer());
    if (ai) {
        ui->wonderSelectionPage->setEnabled(false);
        auto chosen = ai->chooseWonderFromDraft(availableWonders);
        if (chosen) {
            QTimer::singleShot(1000, this, [this, chosen]() {
                onWonderChosen(chosen->getId());
            });
            return;
        }
    } else {
        ui->wonderSelectionPage->setEnabled(true);
    }

    ui->wonderSelectionPage->setWonders(ids, names, colors, costs, effects);
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

// Helper to find node
CardNode* findCardNode(Game* game, int cardId) {
    if (!game) return nullptr;
    const auto& rows = game->getBoard().getPyramid().getRows();
    for (const auto& row : rows) {
        for (const auto& node : row) {
            if (node && node->getCard() && node->getCard()->getId() == cardId) {
                return const_cast<CardNode*>(node.get());
            }
        }
    }
    return nullptr;
}

void MainWindow::onCardSelected(int cardId)
{
    m_selectedCardId = cardId;
    CardNode* node = findCardNode(m_game, cardId);

    if (!node || !node->isPlayable() || node->isPlayed()) {
        ui->lblCost->setText("Unavailable");
        ui->btnBuild->setEnabled(false);
        ui->btnDiscard->setEnabled(false);
        ui->btnWonder->setEnabled(false);
        return;
    }

    Player* p = m_game->getCurrentPlayer();
    Player* opp = m_game->getOpponent();
    auto card = node->getCard();

    int cost = p->findTotalCost(*card, *opp);
    
    if (cost == GameConstants::IMPOSSIBLE_COST) {
        ui->lblCost->setText("Cost: ❌");
        ui->btnBuild->setEnabled(false);
    } else {
        ui->lblCost->setText("Cost: " + QString::number(cost));
        ui->btnBuild->setEnabled(true);
    }

    ui->btnDiscard->setEnabled(true);

    bool hasAvailableWonder = false;
    for (const auto& w : p->getWonders()) {
        if (!w->getIsBuilt()) {
             int wCost = p->findTotalCost(*w, *opp);
             if (wCost != GameConstants::IMPOSSIBLE_COST) {
                 hasAvailableWonder = true;
                 break;
             }
        }
    }
    ui->btnWonder->setEnabled(hasAvailableWonder);
}

void MainWindow::onBuildClicked()
{
    if (m_selectedCardId == -1) return;
    CardNode* node = findCardNode(m_game, m_selectedCardId);
    if (!node) return;

    Player* p = m_game->getCurrentPlayer();
    Player* opp = m_game->getOpponent();

    if (p->buyCard(node->getCard(), *opp, m_game->getBoard())) {
        node->updatePlayedStatus(true);
        m_game->getBoard().updateVisibility();
        
        if (!p->hasExtraTurn()) {
            m_game->switchTurn();
        } else {
            p->setHasExtraTurn(false);
            QMessageBox::information(this, "Divine Intervention", "You get an extra turn!");
        }
        updateGameState();
    } else {
        QMessageBox::warning(this, "Error", "Could not buy card (unknown error).");
    }
}

void MainWindow::onDiscardClicked()
{
    if (m_selectedCardId == -1) return;
    CardNode* node = findCardNode(m_game, m_selectedCardId);
    if (!node) return;
    
    Player* p = m_game->getCurrentPlayer();
    p->discardCard(node->getCard(), m_game->getBoard());
    
    node->updatePlayedStatus(true);
    m_game->getBoard().updateVisibility();
    
    if (!p->hasExtraTurn()) {
        m_game->switchTurn();
    }
    updateGameState();
}

void MainWindow::onWonderClicked()
{
    if (m_selectedCardId == -1) return;
    CardNode* node = findCardNode(m_game, m_selectedCardId);
    if (!node) return;

    Player* p = m_game->getCurrentPlayer();
    Player* opp = m_game->getOpponent();

    std::vector<Wonder*> availableWonders;
    for (const auto& w : p->getWonders()) {
        if (!w->getIsBuilt()) {
            availableWonders.push_back(w.get());
        }
    }

    if (availableWonders.empty()) return;

    WonderChoiceDialog dialog(this);
    dialog.setWonders(availableWonders, p, opp);
    
    if (dialog.exec() != QDialog::Accepted) return;

    Wonder* selectedWonder = nullptr;
    int wId = dialog.getSelectedWonderId();
    for(auto* w : availableWonders) {
        if(w->getId() == wId) {
            selectedWonder = w;
            break;
        }
    }

    if (!selectedWonder) return;

    if (p->constructWonder(node->getCard(), *selectedWonder, *opp, m_game->getBoard())) {
        node->updatePlayedStatus(true);
        m_game->getBoard().updateVisibility();

        if (p->hasExtraTurn()) { 
             p->setHasExtraTurn(false);
             QMessageBox::information(this, "Divine Intervention", "Wonder Grant: Extra Turn!");
        } else {
             m_game->switchTurn();
        }
        updateGameState();
    } else {
        QMessageBox::warning(this, "Error", "Could not build wonder.");
    }
}

void MainWindow::onDiscardedCardsClicked()
{
    if (!m_game) return;

    const auto& discardedCardsShared = m_game->getDiscardedCards();
    std::vector<const CardBase*> discardedCards;
    for (const auto& cardPtr : discardedCardsShared)
    {
        discardedCards.push_back(cardPtr.get());
    }

    DiscardedCardsDialog dialog(discardedCards, this);
    dialog.exec();
}

void MainWindow::updatePlayerInventories()
{
    if (!m_game) return;

    const Player& p1 = m_game->getPlayer1();
    const Player& p2 = m_game->getPlayer2();

    std::cout << "[UI Update] P1 @" << &p1 << " (" << p1.getName() << ") has " << p1.getWonders().size() << " wonders.\n";
    std::cout << "[UI Update] P2 @" << &p2 << " (" << p2.getName() << ") has " << p2.getWonders().size() << " wonders.\n";

    ui->playerDashboard->updateDashboard(
        p1.getName(), 
        p1.getCoins(), 
        p1.getResources(),
        p1.getWonders()
    );

    ui->opponentDashboard->updateDashboard(
        p2.getName(), 
        p2.getCoins(), 
        p2.getResources(),
        p2.getWonders()
    );

    ui->militaryTrackWidget->updatePawnPosition(m_game->getBoard().getMilitaryTrack().getPawnPosition());

    QLayoutItem* item;
    while ((item = ui->progressTokensLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    auto addPlayerTokens = [this](const Player& p) {
        for (const auto& tokenPtr : p.getProgressTokens()) {
            QLabel* tokenLabel = new QLabel("⚡ " + QString::fromStdString(tokenPtr->getName()), this);
            tokenLabel->setStyleSheet(
                "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #FFD700, stop:0.5 #FFF9C4, stop:1 #FFD700); "
                "color: #3E2723; padding: 8px; border: 2px solid #8B4513; border-radius: 6px; font-weight: bold; font-family: 'Times New Roman';"
            );
            ui->progressTokensLayout->addWidget(tokenLabel);
        }
    };

    addPlayerTokens(p1);
    addPlayerTokens(p2);
}

void MainWindow::updateTurnIndicator()
{
    if (!m_game) return;
    bool isPlayer1Turn = (m_game->getCurrentPlayer()->getName() == m_game->getPlayer1().getName());
    QString borderColor = isPlayer1Turn ? "#1565C0" : "#8B0000"; 
    QString arrow = isPlayer1Turn ? "⬇️" : "⬆️";
    ui->turnIndicator->setText("🦆 " + arrow);
    ui->turnIndicator->setStyleSheet(
        QString("background-color: #FFD700; border: 5px solid %1; border-radius: 30px; font-size: 16px; font-weight: bold; color: #2C1810;").arg(borderColor)
    );
}

QString MainWindow::getColorHex(Color c)
{
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

void MainWindow::renderGame()
{
    if (!m_game) return;
    ui->boardWidgetPage->clearBoard();
    Board& boardData = m_game->getBoard();
    const auto& rows = boardData.getPyramid().getRows();
    
    auto formatCost = [](const std::map<Resource, uint8_t>& costMap) -> QString {
        QStringList parts;
        for (auto const& [res, count] : costMap) {
             QString icon;
             switch(res) {
                 case Resource::Wood: icon = "wood.png"; break;
                 case Resource::Clay: icon = "clay.png"; break;
                 case Resource::Stone: icon = "stone.png"; break;
                 case Resource::Glass: icon = "glass.png"; break;
                 case Resource::Papyrus: icon = "papyrus.png"; break;
                 case Resource::Coin: icon = "coin.png"; break;
             }
             if (!icon.isEmpty()) {
                parts << QString("%1 x <img src=':/resources/UI/%2' height='14'>").arg(count).arg(icon);
             }
        }
        return parts.join("<br>");
    };

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
            QString cost = formatCost(cardPtr->getCost());
            QString effect = QString::fromStdString(cardPtr->getEffectDescription());
            ui->boardWidgetPage->placeCard(id, name, getColorHex(col), isFaceUp, static_cast<int>(r), static_cast<int>(c), cardsInRow, cost, effect);
        }
    }
}