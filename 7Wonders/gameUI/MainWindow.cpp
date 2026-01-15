#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "CardBase.h"
#include "Wonder.h"
#include "CardWidget.h"
#include "BoardWidget.h"
#include "WonderSelectionWidget.h"
#include "MilitaryTrackWidget.h"
#include "SplashScreen.h"
#include "PlayerDashboardWidget.h"
#include "Game.h"
#include "GameConstants.h"
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
        "background: qradialgradient(cx:0.5, cy:0.5, radius:0.8, "
        "  fx:0.5, fy:0.5, stop:0 #4A3728, stop:1 #2C1810); "
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
    connect(ui->splashScreenPage, &SplashScreen::startGame, this, &MainWindow::onSplashFinished);

    ui->statusbar->showMessage("⚔️ Press ENTER to begin your conquest! ⚔️");

    // Hide zones initially
    ui->opponentDashboard->setVisible(false);
    ui->playerDashboard->setVisible(false);
    ui->rightZone->setVisible(false);

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

void MainWindow::onSplashFinished()
{
    ui->opponentDashboard->setVisible(true);
    ui->playerDashboard->setVisible(true);
    ui->rightZone->setVisible(true);

    ui->statusbar->showMessage("⚔️ Initializing the ancient world of ducks... ⚔️");
    QTimer::singleShot(100, this, &MainWindow::startGame);
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
    
    // Check for end of age
    if (m_game->isEndOfAge()) {
        QMessageBox::information(this, "Age Complete", "The current age has ended!");
        m_game->startNextAge();
        if (m_game->isGameOver()) {
             QMessageBox::information(this, "Game Over", "The game has ended!");
             // TODO: Show score screen
             return;
        }
        // Refresh for new age
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
        // TODO: Show nice game over screen
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
             QString rName;
             switch(res) {
                 case Resource::Wood: rName = "Wood"; break;
                 case Resource::Clay: rName = "Clay"; break;
                 case Resource::Stone: rName = "Stone"; break;
                 case Resource::Glass: rName = "Glass"; break;
                 case Resource::Papyrus: rName = "Papyrus"; break;
                 case Resource::Coin: rName = "Coin"; break;
             }
             parts << QString("%1 %2").arg(count).arg(rName);
        }
        return parts.join("\n");
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

    // Determine costs
    Player* p = m_game->getCurrentPlayer();
    Player* opp = m_game->getOpponent();
    auto card = node->getCard();

    // Check Build Cost
    int cost = p->findTotalCost(*card, *opp);
    
    if (cost == GameConstants::IMPOSSIBLE_COST) {
        ui->lblCost->setText("Cost: ❌");
        ui->btnBuild->setEnabled(false);
    } else {
        ui->lblCost->setText("Cost: " + QString::number(cost));
        ui->btnBuild->setEnabled(true);
    }

    // Always allowed to discard if card is playable
    ui->btnDiscard->setEnabled(true);

    // Check Wonder Construction
    // Does player have any unbuilt wonders?
    bool hasAvailableWonder = false;
    for (const auto& w : p->getWonders()) {
        if (!w->getIsBuilt()) {
             // Check if we can afford the wonder using this card as material (card itself is free, we pay wonder cost)
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

    bool success = p->buyCard(node->getCard(), *opp, m_game->getBoard());
    if (success) {
        node->updatePlayedStatus(true);
        m_game->getBoard().updateVisibility();
        
        if (!p->hasExtraTurn()) {
            m_game->switchTurn();
        } else {
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
    p->discardCard(*node->getCard());
    
    node->updatePlayedStatus(true);
    m_game->getBoard().updateVisibility();
    
    // Discard rarely grants extra turns unless specific wonders/tokens effect?
    // Usually discard passes turn.
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

    Wonder* selectedWonder = nullptr;
    if (availableWonders.size() == 1) {
        selectedWonder = availableWonders[0];
    } else {
        // Ask user to pick
        QStringList items;
        for (auto* w : availableWonders) {
            int cost = p->findTotalCost(*w, *opp);
            QString item = QString::fromStdString(w->getName());
            if (cost == GameConstants::IMPOSSIBLE_COST) {
                item += " (Too Expensive)";
            } else {
                item += " (Cost: " + QString::number(cost) + ")";
            }
            items << item;
        }

        bool ok;
        QString item = QInputDialog::getItem(this, "Select Wonder", 
                                             "Choose a Wonder to build:", items, 0, false, &ok);
        if (ok && !item.isEmpty()) {
            int index = items.indexOf(item);
            if (index >= 0 && index < availableWonders.size()) {
                selectedWonder = availableWonders[index];
            }
        } else {
            return; // Cancelled
        }
    }

    if (!selectedWonder) return;

    // Check cost again
    int cost = p->findTotalCost(*selectedWonder, *opp);
    if (cost == GameConstants::IMPOSSIBLE_COST) {
        QMessageBox::warning(this, "Too Expensive", "You cannot afford this wonder!");
        return;
    }

    p->constructWonder(node->getCard(), *selectedWonder, *opp, m_game->getBoard());
    
    // Check 7 Wonders rule logic (game->handle7WondersRule is private, might need to reimplement or make public)
    // For now, ignoring auto-discard of 8th wonder, but UI should ideally handle it.
    
    node->updatePlayedStatus(true);
    m_game->getBoard().updateVisibility();

    if (p->hasExtraTurn()) { 
         QMessageBox::information(this, "Divine Intervention", "Wonder Grant: Extra Turn!");
    } else {
         m_game->switchTurn();
    }
    
    updateGameState();
}


void MainWindow::updatePlayerInventories()
{
    if (!m_game) return;

    const Player* player = m_game->getCurrentPlayer();
    const Player* opponent = m_game->getOpponent();

    if (!player || !opponent) return;

    // Use shared_ptr<Wonder> because Player usually holds shared_ptr
    ui->playerDashboard->updateDashboard(
        player->getName(), 
        player->getCoins(), 
        player->getWonders()
    );

    ui->opponentDashboard->updateDashboard(
        opponent->getName(), 
        opponent->getCoins(), 
        opponent->getWonders()
    );

    ui->militaryTrackWidget->updatePawnPosition(m_game->getBoard().getMilitaryTrack().getPawnPosition());

    // Clear and rebuild tokens
    QLayoutItem* item;
    while ((item = ui->progressTokensLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

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

            ui->progressTokensLayout->addWidget(tokenLabel);
        }
    };

    addPlayerTokens(player);
    addPlayerTokens(opponent);
}

void MainWindow::updateTurnIndicator()
{
    if (!m_game) return;

    // Check if it is Player 1's turn (The human player, usually "Player 1")
    bool isPlayer1Turn = (m_game->getCurrentPlayer()->getName() == m_game->getPlayer1().getName());
    
    // Player 1 = Blue (#1565C0), Opponent = Red (#8B0000)
    QString borderColor = isPlayer1Turn ? "#1565C0" : "#8B0000"; 
    
    // We can also change the icon slightly or add an arrow
    QString arrow = isPlayer1Turn ? "⬇️" : "⬆️"; // Pointing to Player (Bottom) or Opponent (Top)

    ui->turnIndicator->setText("🦆 " + arrow);
    
    ui->turnIndicator->setStyleSheet(
        QString("background-color: #FFD700; "
                "border: 5px solid %1; "
                "border-radius: 30px; "
                "font-size: 16px; "
                "font-weight: bold; "
                "color: #2C1810;")
        .arg(borderColor)
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
             QString rName;
             switch(res) {
                 case Resource::Wood: rName = "Wood"; break;
                 case Resource::Clay: rName = "Clay"; break;
                 case Resource::Stone: rName = "Stone"; break;
                 case Resource::Glass: rName = "Glass"; break;
                 case Resource::Papyrus: rName = "Papyrus"; break;
                 case Resource::Coin: rName = "Coin"; break;
             }
             parts << QString("%1 %2").arg(count).arg(rName);
        }
        return parts.join("\n");
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

            ui->boardWidgetPage->placeCard(id, name, getColorHex(col), isFaceUp,
                static_cast<int>(r), static_cast<int>(c), cardsInRow, cost, effect);
        }
    }
}
