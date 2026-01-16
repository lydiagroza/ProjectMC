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

    ui->opponentDashboard->setTheme(true);
    ui->playerDashboard->setTheme(false);

    connect(ui->boardWidgetPage, &BoardWidget::cardClicked, this, &MainWindow::onCardSelected);
    connect(ui->btnBuild, &QPushButton::clicked, this, &MainWindow::onBuildClicked);
    connect(ui->btnDiscard, &QPushButton::clicked, this, &MainWindow::onDiscardClicked);
    connect(ui->btnWonder, &QPushButton::clicked, this, &MainWindow::onWonderClicked);
    connect(ui->wonderSelectionPage, &WonderSelectionWidget::wonderChosen, this, &MainWindow::onWonderChosen);
    connect(ui->splashScreenPage, &SplashScreen::gameModeSelected, this, &MainWindow::onSplashFinished);
    connect(ui->nameSelectionPage, &NameSelectionWidget::namesConfirmed, this, &MainWindow::onNamesConfirmed);

    ui->stack->setCurrentIndex(0);
    showMaximized();
}

MainWindow::~MainWindow()
{
    delete m_game;
    delete ui;
}

void MainWindow::processAITurn()
{
    AI_Player* ai = dynamic_cast<AI_Player*>(m_game->getCurrentPlayer());
    if (!ai || m_game->isGameOver()) return;

    bool hadExtraTurn = ai->hasExtraTurn();
    ai->makeDecision(m_game->getBoard(), *m_game->getOpponent(), m_game->getCurrentAge());

    if (hadExtraTurn) {
        ai->setHasExtraTurn(false);
        m_game->switchTurn();
    }
    else if (ai->hasExtraTurn()) {
        ai->setHasExtraTurn(false);
        QMessageBox::information(this, "AI Bonus", "AI gets an extra turn!");
    }
    else {
        m_game->switchTurn();
    }

    updateGameState();
}

void MainWindow::onBuildClicked()
{
    if (m_selectedCardId == -1) return;
    CardNode* node = findCardNode(m_game, m_selectedCardId);
    if (!node) return;

    Player* p = m_game->getCurrentPlayer();
    Player* opp = m_game->getOpponent();
    bool hadExtraTurn = p->hasExtraTurn();

    if (p->buyCard(node->getCard(), *opp, m_game->getBoard())) {
        node->updatePlayedStatus(true);
        m_game->getBoard().updateVisibility();

        if (hadExtraTurn) {
            p->setHasExtraTurn(false);
            m_game->switchTurn();
        }
        else if (p->hasExtraTurn()) {
            p->setHasExtraTurn(false);
            QMessageBox::information(this, "Bonus", "You get an extra turn!");
        }
        else {
            m_game->switchTurn();
        }
        updateGameState();
    }
}

void MainWindow::onWonderClicked()
{
    if (m_selectedCardId == -1) return;
    CardNode* node = findCardNode(m_game, m_selectedCardId);
    if (!node) return;

    Player* p = m_game->getCurrentPlayer();
    Player* opp = m_game->getOpponent();
    bool hadExtraTurn = p->hasExtraTurn();

    std::vector<Wonder*> available;
    for (auto& w : p->getWonders())
        if (!w->getIsBuilt()) available.push_back(w.get());

    if (available.empty()) return;

    WonderChoiceDialog dlg(this);
    dlg.setWonders(available, p, opp);

    if (dlg.exec() != QDialog::Accepted) return;

    Wonder* selected = nullptr;
    for (auto* w : available)
        if (w->getId() == dlg.getSelectedWonderId()) selected = w;

    if (!selected) return;

    if (p->constructWonder(node->getCard(), *selected, *opp, m_game->getBoard())) {
        node->updatePlayedStatus(true);
        m_game->getBoard().updateVisibility();

        if (hadExtraTurn) {
            p->setHasExtraTurn(false);
            m_game->switchTurn();
        }
        else if (p->hasExtraTurn()) {
            p->setHasExtraTurn(false);
            QMessageBox::information(this, "Wonder Bonus", "Extra Turn!");
        }
        else {
            m_game->switchTurn();
        }
        updateGameState();
    }
    else {
        QMessageBox::warning(this, "Error", "Could not build wonder.");
    }
}
