#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "CardBase.h"
#include "Wonder.h"
#include "CardWidget.h"
#include "BoardWidget.h"
#include "WonderSelectionWidget.h"
#include "MilitaryTrackWidget.h"
#include "SplashScreen.h"
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
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_game(new Game())
    , m_selectedCardId(-1)
    , m_draftPhase(0)
{
    ui->setupUi(this);

    // Apply Shadows manually as they are tricky in .ui
    QGraphicsDropShadowEffect* oppShadow = new QGraphicsDropShadowEffect();
    oppShadow->setBlurRadius(8);
    oppShadow->setColor(QColor(0, 0, 0, 180));
    oppShadow->setOffset(2, 2);
    ui->opponentLabel->setGraphicsEffect(oppShadow);

    QGraphicsDropShadowEffect* playerShadow = new QGraphicsDropShadowEffect();
    playerShadow->setBlurRadius(8);
    playerShadow->setColor(QColor(0, 0, 0, 180));
    playerShadow->setOffset(2, 2);
    ui->playerLabel->setGraphicsEffect(playerShadow);

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

    // Apply style to BoardWidget manually if needed, or rely on .ui inheritance
    ui->boardWidgetPage->setStyleSheet(
        "background: qradialgradient(cx:0.5, cy:0.5, radius:0.8, "
        "  fx:0.5, fy:0.5, stop:0 #4A3728, stop:1 #2C1810); "
        "border: 4px solid #8B4513; "
        "border-radius: 15px;"
    );

    // Connect Signals
    // Access widgets via ui pointer
    connect(ui->boardWidgetPage, &BoardWidget::cardClicked, this, &MainWindow::onCardSelected);
    connect(ui->btnConstruct, &QPushButton::clicked, this, &MainWindow::onConstructClicked);
    connect(ui->wonderSelectionPage, &WonderSelectionWidget::wonderChosen, this, &MainWindow::onWonderChosen);
    connect(ui->splashScreenPage, &SplashScreen::startGame, this, &MainWindow::onSplashFinished);

    ui->statusbar->showMessage("⚔️ Press ENTER to begin your conquest! ⚔️");

    // Hide zones initially
    ui->opponentZone->setVisible(false);
    ui->playerZone->setVisible(false);
    ui->rightZone->setVisible(false);

    // Start at Splash Screen (Index 0)
    ui->stack->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete m_game;
    delete ui;
}

void MainWindow::onSplashFinished()
{
    ui->opponentZone->setVisible(true);
    ui->playerZone->setVisible(true);
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
    ui->statusbar->showMessage("⚔️ " + currentPlayerName + " must choose a wonder! ⚔️", 5000);

    ui->wonderSelectionPage->setWonders(ids, names, colors);
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

    QString playerText = QString("🏛️ %1 | 🦆 %2 ducks")
        .arg(QString::fromStdString(player->getName()))
        .arg(player->getCoins());
    ui->playerLabel->setText(playerText);

    QString opponentText = QString("⚔️ %1 | 🦆 %2 ducks")
        .arg(QString::fromStdString(opponent->getName()))
        .arg(opponent->getCoins());
    ui->opponentLabel->setText(opponentText);

    // Logic to update wonder widgets in the placeholder layouts
    auto setupWonders = [this](const Player* p, QHBoxLayout* wondersLayout) {
        if (!wondersLayout) return;

        QLayoutItem* item;
        while ((item = wondersLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }

        for (const auto& wonderPtr : p->getWonders()) {
            // We need a parent widget, use the layout's parent
            QWidget* parent = wondersLayout->parentWidget();
            CardWidget* wonderWidget = new CardWidget(wonderPtr->getId(), parent);
            wonderWidget->setFixedSize(70, 95);

            QString name = QString::fromStdString(wonderPtr->getName());
            QString color = wonderPtr->getIsBuilt() ? "#DAA520" : "#6D4C41";

            wonderWidget->setupCard(name, color, true);
            wondersLayout->addWidget(wonderWidget);
        }
    };

    setupWonders(player, ui->playerWondersLayout);
    setupWonders(opponent, ui->opponentWondersLayout);

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

            ui->boardWidgetPage->placeCard(id, name, getColorHex(col), isFaceUp,
                static_cast<int>(r), static_cast<int>(c), cardsInRow);
        }
    }
}
