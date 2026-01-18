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
#include "GameEndDialog.h"
#include "SaveManager.h"
#include "GameSelectionDialog.h"
#include "SoundManager.h"
#include <QDateTime>
#include <QInputDialog>

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

static CardNode* findCardNodeInternal(Game* game, int cardId) {
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

    ui->boardWidgetPage->setStyleSheet(
        "background: transparent; "
        "border: 4px solid #8B4513; "
        "border-radius: 15px;"
    );

    ui->opponentDashboard->setTheme(true);
    ui->playerDashboard->setTheme(false);

    connect(ui->boardWidgetPage, &BoardWidget::cardClicked, this, &MainWindow::onCardSelected);
    
    connect(ui->btnBuild, &QPushButton::clicked, this, &MainWindow::onBuildClicked);
    connect(ui->btnDiscard, &QPushButton::clicked, this, &MainWindow::onDiscardClicked);
    connect(ui->btnWonder, &QPushButton::clicked, this, &MainWindow::onWonderClicked);
    
    connect(ui->wonderSelectionPage, &WonderSelectionWidget::wonderChosen, this, &MainWindow::onWonderChosen);
    connect(ui->splashScreenPage, &SplashScreen::gameModeSelected, this, &MainWindow::onSplashFinished);
    connect(ui->nameSelectionPage, &NameSelectionWidget::namesConfirmed, this, &MainWindow::onNamesConfirmed);
    connect(ui->btnDiscardedCards, &QPushButton::clicked, this, &MainWindow::onDiscardedCardsClicked);

    ui->statusbar->showMessage("⚔️ Welcome to 7 Wonders Duel! ⚔️");

    ui->opponentDashboard->setVisible(false);
    ui->playerDashboard->setVisible(false);
    ui->rightZone->setVisible(false);
    ui->btnDiscardedCards->setVisible(false);
    ui->turnContainer->setVisible(false);

    ui->btnBuild->setEnabled(false);
    ui->btnDiscard->setEnabled(false);
    ui->btnWonder->setEnabled(false);

    ui->stack->setCurrentIndex(0);

    rebuildRightPanel();

    this->showMaximized();

    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
    m_player->setSource(QUrl("qrc:/resources/UI/quack.mp3"));
    m_audioOutput->setVolume(1.0);

    m_musicPlayer = new QMediaPlayer(this);
    m_musicOutput = new QAudioOutput(this);
    m_musicPlayer->setAudioOutput(m_musicOutput);
    m_musicPlayer->setSource(QUrl("qrc:/resources/UI/background_music.mp3"));
    m_musicOutput->setVolume(0.5);
    m_musicPlayer->setLoops(QMediaPlayer::Infinite);
    m_musicPlayer->play();
}

void MainWindow::rebuildRightPanel()
{
    QVBoxLayout* rLayout = qobject_cast<QVBoxLayout*>(ui->rightZone->layout());
    if (!rLayout) return;

    ui->militaryTrackWidget->setParent(nullptr); 
    ui->actionsContainer->setParent(nullptr);

    QLayoutItem* item;
    while ((item = rLayout->takeAt(0)) != nullptr) {
        if (item->widget() && item->widget() != ui->militaryTrackWidget && item->widget() != ui->actionsContainer) {
            delete item->widget();
        }
        delete item;
    }

    QFrame* splitContainer = new QFrame(ui->rightZone);
    QHBoxLayout* splitLayout = new QHBoxLayout(splitContainer);
    splitLayout->setContentsMargins(0, 0, 0, 0);
    splitLayout->setSpacing(5);

    ui->militaryTrackWidget->setParent(splitContainer);
    splitLayout->addWidget(ui->militaryTrackWidget);

    QFrame* progressContainer = new QFrame(splitContainer);
    QVBoxLayout* progressCol = new QVBoxLayout(progressContainer);
    progressCol->setContentsMargins(0, 0, 0, 0);
    progressCol->setSpacing(2);

    m_lblProgressP2 = new QLabel("Opponent", progressContainer);
    m_lblProgressP2->setStyleSheet("color: #B71C1C; font-weight: bold; font-size: 10px;");
    m_lblProgressP2->setAlignment(Qt::AlignCenter);
    progressCol->addWidget(m_lblProgressP2);

    m_p2ProgressLayout = new QVBoxLayout(); 
    progressCol->addLayout(m_p2ProgressLayout);

    QFrame* line = new QFrame(progressContainer);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("color: #8B4513; margin: 5px;");
    progressCol->addWidget(line);

    m_lblProgressP1 = new QLabel("You", progressContainer);
    m_lblProgressP1->setStyleSheet("color: #1565C0; font-weight: bold; font-size: 10px;");
    m_lblProgressP1->setAlignment(Qt::AlignCenter);
    progressCol->addWidget(m_lblProgressP1);

    m_p1ProgressLayout = new QVBoxLayout(); 
    progressCol->addLayout(m_p1ProgressLayout);

    QPushButton* infoBtn = new QPushButton("?", progressContainer);
    infoBtn->setFixedSize(24, 24);
    infoBtn->setCursor(Qt::PointingHandCursor);
    infoBtn->setStyleSheet(
        "QPushButton { background: #FFD700; color: #2C1810; border-radius: 12px; font-weight: bold; border: 2px solid #DAA520; font-size: 12px; }"
        "QPushButton:hover { background: #FFEA00; }"
    );
    connect(infoBtn, &QPushButton::clicked, this, &MainWindow::onProgressInfoClicked);
    
    progressCol->addStretch();
    progressCol->addWidget(infoBtn, 0, Qt::AlignCenter);

    progressContainer->setFixedWidth(60); 

    splitLayout->addWidget(progressContainer);

    rLayout->addWidget(splitContainer, 1); 

    ui->actionsContainer->setParent(ui->rightZone);
    rLayout->addWidget(ui->actionsContainer, 0);

    m_duckBtn = new QPushButton("🦆 Ask Duck", ui->rightZone);
    m_duckBtn->setCursor(Qt::PointingHandCursor);
    m_duckBtn->setStyleSheet(
        "QPushButton { background: #FFD700; color: #2C1810; border: 2px solid #DAA520; border-radius: 15px; padding: 8px; font-weight: bold; font-size: 14px; }"
        "QPushButton:hover { background: #FFEA00; border-color: #FF8F00; }"
    );
    connect(m_duckBtn, &QPushButton::clicked, this, &MainWindow::onDuckHintClicked);
    rLayout->addWidget(m_duckBtn, 0, Qt::AlignCenter);

    QPushButton* saveBtn = new QPushButton("Save Game", ui->rightZone);
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setStyleSheet(
        "QPushButton { background: #5D4037; color: #F5E6D3; border: 2px solid #8B4513; border-radius: 10px; padding: 5px; font-weight: bold; }"
        "QPushButton:hover { background: #8D6E63; border-color: #FFD700; }"
    );
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveGameClicked);
    rLayout->addWidget(saveBtn, 0, Qt::AlignCenter);
}

QString MainWindow::getTokenDescription(const QString& tokenName)
{
    QString upperName = tokenName.toUpper().trimmed();
    
    if (upperName == "AGRICULTURE") return "Agriculture: Gain 6 coins and 4 victory points.";
    if (upperName == "ARCHITECTURE") return "Architecture: Building wonders costs 2 fewer resources.";
    if (upperName == "ECONOMY") return "Economy: Gain money when your opponent trades.";
    if (upperName == "LAW") return "Law: This token is worth one scientific symbol.";
    if (upperName == "MASONRY") return "Masonry: Building blue cards costs 2 fewer resources.";
    if (upperName == "MATHEMATICS") return "Mathematics: 3 victory points for each progress token (including this one).";
    if (upperName == "PHILOSOPHY") return "Philosophy: Gain 7 victory points.";
    if (upperName == "STRATEGY") return "Strategy: Gain an extra military building.";
    if (upperName == "THEOLOGY") return "Theology: Your wonders get the 'Extra Turn' effect.";
    if (upperName == "URBANISM") return "Urbanism: Gain 6 coins. Building chains is free (even if you don't have the symbol).";
    
    qDebug() << "[UI] Warning: No description for token:" << tokenName;
    return "Description unavailable (" + tokenName + ").";
}

void MainWindow::onSaveGameClicked()
{
    if (!m_game) return;

    bool ok;
    QString fileName = QInputDialog::getText(this, "Save Game",
                                         "Save Name:", QLineEdit::Normal,
                                         "Save_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmm"), &ok);
    if (ok && !fileName.isEmpty()) {
        if (!fileName.endsWith(".7wd")) fileName += ".7wd";
        
        QDir dir("saves");
        if (!dir.exists()) dir.mkpath(".");
        
        if (SaveManager::saveGame(m_game, dir.filePath(fileName))) {
            QMessageBox::information(this, "Success", "Game saved successfully!");
        } else {
            QMessageBox::warning(this, "Error", "Failed to save game.");
        }
    }
}

void MainWindow::onProgressInfoClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Progress Tokens");
    dialog.setStyleSheet("background: #3E2723; color: #F5E6D3; font-family: 'Times New Roman';");
    dialog.setMinimumSize(500, 600);

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);
    QLabel* title = new QLabel("Available Progress Tokens", &dialog);
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #FFD700; margin-bottom: 15px;");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    QScrollArea* scroll = new QScrollArea(&dialog);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("background: transparent; border: none;");
    
    QWidget* container = new QWidget();
    container->setStyleSheet("background: transparent;");
    QGridLayout* gridLayout = new QGridLayout(container);
    gridLayout->setSpacing(20);
    gridLayout->setContentsMargins(20, 20, 20, 20);

    const auto& availableTokens = m_game->getBoard().getAvailableProgressTokens();
    
            auto getTokenSymbol = [](const QString& name) -> QString {
                QString n = name.toUpper().trimmed();
                if (n == "AGRICULTURE") return "Ag";
                if (n == "ARCHITECTURE") return "Ar";
                if (n == "ECONOMY") return "Ec";
                if (n == "LAW") return "La";
                if (n == "MASONRY") return "Ms";
                if (n == "MATHEMATICS") return "Mt";
                if (n == "PHILOSOPHY") return "Ph";
                if (n == "STRATEGY") return "St";
                if (n == "THEOLOGY") return "Th";
                if (n == "URBANISM") return "Ur";
                return "??";
            };    
        if (availableTokens.empty()) {
            QLabel* emptyLbl = new QLabel("No more tokens available.");
            emptyLbl->setStyleSheet("font-size: 16px; color: #BBB;");
            emptyLbl->setAlignment(Qt::AlignCenter);
            gridLayout->addWidget(emptyLbl, 0, 0);
        } else {
            int row = 0;
            int col = 0;
            int maxCols = 2;
    
            for (const auto& tokenPtr : availableTokens) {
                QString name = QString::fromStdString(tokenPtr->getName());
                
                QPushButton* tokenBtn = new QPushButton();
                tokenBtn->setFixedSize(140, 160);
                tokenBtn->setCursor(Qt::PointingHandCursor);
                tokenBtn->setStyleSheet(
                    "QPushButton { "
                    "  background: #4E342E; "
                    "  border: 2px solid #8B4513; "
                    "  border-radius: 15px; "
                    "}"
                    "QPushButton:hover { "
                    "  background: #5D4037; "
                    "  border-color: #FFD700; "
                    "}"
                );
    
                QVBoxLayout* btnLayout = new QVBoxLayout(tokenBtn);
                btnLayout->setContentsMargins(10, 10, 10, 10);
                
                QLabel* coin = new QLabel(getTokenSymbol(name));
                coin->setFixedSize(80, 80);
                coin->setAlignment(Qt::AlignCenter);
                coin->setStyleSheet(
                    "QLabel { "
                    "  background: qradialgradient(cx:0.5, cy:0.5, radius: 0.5, fx:0.5, fy:0.5, stop:0 #A5D6A7, stop:1 #1B5E20); "
                    "  color: #FFFFFF; "
                    "  border: 4px solid #2E7D32; "
                    "  border-radius: 40px; "
                    "  font-family: 'Arial', sans-serif; "
                    "  font-size: 32px; font-weight: bold;"
                    "  background-color: transparent;"
                    "}"
                );
            QLabel* nameLbl = new QLabel(name);
            nameLbl->setWordWrap(true);
            nameLbl->setAlignment(Qt::AlignCenter);
            nameLbl->setStyleSheet("color: #FFD700; font-weight: bold; font-size: 14px; background: transparent; border: none;");

            btnLayout->addWidget(coin, 0, Qt::AlignCenter);
            btnLayout->addWidget(nameLbl, 0, Qt::AlignCenter);

            connect(tokenBtn, &QPushButton::clicked, this, [this, name]() {
                QMessageBox msgBox;
                msgBox.setWindowTitle("Token Details");
                msgBox.setText("<h2 style='color:#FFD700'>" + name + "</h2>");
                msgBox.setInformativeText("<span style='font-size:14px; color:#F5E6D3'>" + getTokenDescription(name) + "</span>");
                msgBox.setStyleSheet("QMessageBox { background: #3E2723; } QLabel { color: #F5E6D3; } QPushButton { color: #2C1810; background: #FFD700; }");
                msgBox.exec();
            });

            gridLayout->addWidget(tokenBtn, row, col);

            col++;
            if (col >= maxCols) {
                col = 0;
                row++;
            }
        }
    }
    
    container->setLayout(gridLayout);
    scroll->setWidget(container);
    mainLayout->addWidget(scroll);

    QPushButton* closeBtn = new QPushButton("Close", &dialog);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(
        "QPushButton { "
        "  background: #8B0000; color: white; padding: 10px; border-radius: 10px; font-weight: bold; font-size: 16px; border: 2px solid #B71C1C; "
        "}"
        "QPushButton:hover { background: #B71C1C; }"
    );
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    mainLayout->addWidget(closeBtn);

    dialog.exec();
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
        onNamesConfirmed("AI Player 1", "AI Player 2", 1);
    } else {
        if (mode == SplashScreen::PvP) {
            GameSelectionDialog dlg(this);
            if (dlg.exec() == QDialog::Accepted) {
                if (dlg.isNewGame()) {
                    ui->nameSelectionPage->setMode(true, true);
                    ui->stack->setCurrentWidget(ui->nameSelectionPage);
                    ui->statusbar->showMessage("⚔️ Who dares to challenge the ducks? ⚔️");
                } else {
                    Game* loadedGame = SaveManager::loadGame(dlg.getSelectedFile());
                    if (loadedGame) {
                        delete m_game;
                        m_game = loadedGame;
                        
                        ui->opponentDashboard->setVisible(true);
                        ui->playerDashboard->setVisible(true);
                        ui->rightZone->setVisible(true);
                        ui->btnDiscardedCards->setVisible(true);
                        ui->turnContainer->setVisible(true);
                        
                        updateGameState(); 
                        
                        ui->statusbar->showMessage("⚔️ Game Loaded! ⚔️");
                    } else {
                        QMessageBox::critical(this, "Error", "Failed to load game file!");
                        ui->stack->setCurrentWidget(ui->splashScreenPage);
                    }
                }
            } else {
                 ui->stack->setCurrentWidget(ui->splashScreenPage);
            }
        } else {
            ui->nameSelectionPage->setMode(true, false);
            ui->stack->setCurrentWidget(ui->nameSelectionPage);
            ui->statusbar->showMessage("⚔️ Who dares to challenge the ducks? ⚔️");
        }
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
    ui->btnDiscardedCards->setVisible(true);
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
        if (m_duckBtn) m_duckBtn->setVisible(true);
    }
    updatePlayerInventories();
    updateTurnIndicator();

    if (m_game->checkForInstantWin()) {
        GameEndDialog dialog(m_game, this);
        dialog.exec();
        
        ui->stack->setCurrentWidget(ui->splashScreenPage);
        ui->opponentDashboard->setVisible(false);
        ui->playerDashboard->setVisible(false);
        ui->rightZone->setVisible(false);
        ui->btnDiscardedCards->setVisible(false);
        ui->turnContainer->setVisible(false);
        return; 
    }

    if (m_game->isWaitingForProgressTokenChoice()) {
        auto available = m_game->getBoard().getAvailableProgressTokens();
        if (!available.empty()) {
            AI_Player* ai = dynamic_cast<AI_Player*>(m_game->getCurrentPlayer());
            if (ai) {
                auto chosen = ai->chooseProgressToken(available);
                if (chosen) {
                    m_game->resolveProgressTokenChoice(chosen->getId());
                    QTimer::singleShot(500, this, &MainWindow::updateGameState);
                    return;
                }
            } else {
                QDialog dialog(this);
                dialog.setWindowTitle("Choose a Progress Token");
                dialog.setStyleSheet("background: #3E2723; color: #F5E6D3; font-family: 'Times New Roman';");
                QVBoxLayout* layout = new QVBoxLayout(&dialog);
                
                QLabel* lbl = new QLabel("You got a pair! Choose a token:", &dialog);
                lbl->setStyleSheet("font-size: 16px; font-weight: bold; color: #FFD700;");
                layout->addWidget(lbl);

                int chosenId = -1;
                for (auto t : available) {
                    QPushButton* btn = new QPushButton(QString::fromStdString(t->getName()), &dialog);
                    btn->setStyleSheet("background: #4E342E; color: #A5D6A7; padding: 10px; border: 1px solid #8B4513; font-weight: bold;");
                    btn->setToolTip(getTokenDescription(QString::fromStdString(t->getName())));
                    connect(btn, &QPushButton::clicked, [&dialog, &chosenId, t]() {
                        chosenId = t->getId();
                        dialog.accept();
                    });
                    layout->addWidget(btn);
                }
                
                if (dialog.exec() == QDialog::Accepted && chosenId != -1) {
                    m_game->resolveProgressTokenChoice(chosenId);
                    updatePlayerInventories();
                }
            }
        }
    }

    if (m_game->isWaitingForGreatLibraryChoice()) {
        auto removed = m_game->getBoard().getRemovedProgressTokens();
        if (!removed.empty()) {
            AI_Player* ai = dynamic_cast<AI_Player*>(m_game->getCurrentPlayer());
            if (ai) {
                auto chosen = ai->chooseProgressToken(removed);
                if (chosen) {
                    m_game->resolveGreatLibraryChoice(chosen->getId());
                    updatePlayerInventories();
                    QTimer::singleShot(500, this, &MainWindow::updateGameState);
                    return;
                }
            } else {
                QDialog dialog(this);
                dialog.setWindowTitle("The Great Library");
                dialog.setStyleSheet("background: #3E2723; color: #F5E6D3; font-family: 'Times New Roman';");
                QVBoxLayout* layout = new QVBoxLayout(&dialog);
                
                QLabel* lbl = new QLabel("The Great Library: Choose a token from the box:", &dialog);
                lbl->setStyleSheet("font-size: 16px; font-weight: bold; color: #FFD700;");
                layout->addWidget(lbl);

                int chosenId = -1;
                for (auto t : removed) {
                    QPushButton* btn = new QPushButton(QString::fromStdString(t->getName()), &dialog);
                    btn->setStyleSheet("background: #4E342E; color: #A5D6A7; padding: 10px; border: 1px solid #8B4513; font-weight: bold;");
                    btn->setToolTip(getTokenDescription(QString::fromStdString(t->getName())));
                    connect(btn, &QPushButton::clicked, [&dialog, &chosenId, t]() {
                        chosenId = t->getId();
                        dialog.accept();
                    });
                    layout->addWidget(btn);
                }
                
                if (dialog.exec() == QDialog::Accepted && chosenId != -1) {
                    m_game->resolveGreatLibraryChoice(chosenId);
                    updatePlayerInventories();
                }
            }
        } else {
             m_game->resolveGreatLibraryChoice(-1);
        }
    }
    
    if (m_game->isEndOfAge()) {
        QMessageBox::information(this, "Age Complete", "The current age has ended!");
        m_game->startNextAge();
        if (m_game->isGameOver()) {
             GameEndDialog dialog(m_game, this);
             dialog.exec();
             ui->stack->setCurrentWidget(ui->splashScreenPage);
             return;
        }
        renderGame();
    }

    m_selectedCardId = -1;
    ui->lblCost->setText("Cost: -");
    ui->btnBuild->setEnabled(false);
    ui->btnDiscard->setEnabled(false);
    ui->btnWonder->setEnabled(false);
    
    if (m_game->isGameOver()) {
        GameEndDialog dialog(m_game, this);
        dialog.exec();
        ui->stack->setCurrentWidget(ui->splashScreenPage);
        ui->btnDiscardedCards->setVisible(false);
        ui->turnContainer->setVisible(false);
        return;
    }

    if (m_draftPhase == 0 || m_game->getCurrentDraftSet().empty()) {
        AI_Player* ai = dynamic_cast<AI_Player*>(m_game->getCurrentPlayer());
        if (ai) {
            QTimer::singleShot(800, this, &MainWindow::processAITurn);
        }
    }
}

void MainWindow::startWonderDraft()
{
    if (m_duckBtn) m_duckBtn->setVisible(false);

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

void MainWindow::onCardSelected(int cardId)
{
    m_selectedCardId = cardId;
    CardNode* node = findCardNodeInternal(m_game, cardId);

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
        if (!w->getIsBuilt() && w->getIsAvailable()) {
             int wCost = p->findTotalCost(*w, *opp);
             if (wCost != GameConstants::IMPOSSIBLE_COST) {
                 hasAvailableWonder = true;
                 break;
             }
        }
    }
    ui->btnWonder->setEnabled(hasAvailableWonder);
    
    if (m_game->getNumberOfWondersBuilt() >= 7) {
        ui->btnWonder->setText("7 wonders already built");
    } else {
        ui->btnWonder->setText("Wonder");
    }
}

void MainWindow::onBuildClicked()
{
    SoundManager::instance().playClick();

    if (m_selectedCardId == -1) return;
    CardNode* node = findCardNodeInternal(m_game, m_selectedCardId);
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
    SoundManager::instance().playClick();

    if (m_selectedCardId == -1) return;
    CardNode* node = findCardNodeInternal(m_game, m_selectedCardId);
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
    SoundManager::instance().playClick();

    if (m_selectedCardId == -1) return;
    CardNode* node = findCardNodeInternal(m_game, m_selectedCardId);
    if (!node) return;

    Player* p = m_game->getCurrentPlayer();
    Player* opp = m_game->getOpponent();

    std::vector<Wonder*> availableWonders;
    for (const auto& w : p->getWonders()) {
        if (!w->getIsBuilt() && w->getIsAvailable()) {
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
        m_game->notifyWonderBuilt();
        if (m_player) {
            m_player->stop();
            m_player->play();
        }
        node->updatePlayedStatus(true);
        m_game->getBoard().updateVisibility();

        if (m_game->isWaitingForDiscardChoice()) {
            const auto& discardedCardsShared = m_game->getBuildFromDiscardChoices();
            std::vector<const CardBase*> discardedCards;
            for (const auto& cardPtr : discardedCardsShared) {
                discardedCards.push_back(cardPtr.get());
            }

            DiscardedCardsDialog dialog(discardedCards, this);
            if (dialog.exec() == QDialog::Accepted) {
                int selectedCardId = dialog.getSelectedCardId();
                m_game->resolveBuildFromDiscard(selectedCardId);
            }
        }

        if (m_game->isWaitingForOpponentCardDiscard()) {
            const auto& choices = m_game->getOpponentCardDiscardChoices();
            if (!choices.empty()) {
                std::vector<const CardBase*> cardPtrs;
                for (const auto& c : choices) { cardPtrs.push_back(c.get()); }

                DiscardedCardsDialog dialog(cardPtrs, this);
                dialog.setWindowTitle("Discard Opponent's Card");

                if (dialog.exec() == QDialog::Accepted) {
                    int selectedId = dialog.getSelectedCardId();
                    if (selectedId != -1) {
                        m_game->resolveDiscardOpponentCard(selectedId);
                    }
                }
                else {
                    m_game->resolveDiscardOpponentCard(-1);
                }
            }
        }

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

    const auto& discardedCardsShared = m_game->getBoard().getDiscardPile();
    std::vector<const CardBase*> discardedCards;
    for (const auto& cardPtr : discardedCardsShared)
    {
        discardedCards.push_back(cardPtr.get());
    }

    DiscardedCardsDialog dialog(discardedCards, this);
    dialog.exec();
}

void MainWindow::onDuckHintClicked()
{
    if (m_player) {
        m_player->stop();
        m_player->play();
    }
    
    if (!m_game) return; 
    
    Player* p = m_game->getCurrentPlayer();
    Player* opp = m_game->getOpponent();
    
    QString hint = QString::fromStdString(AI_Player::getBestMoveHint(*p, *opp, m_game->getBoard(), m_game->getCurrentAge(), m_selectedCardId));
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("The wise Duck Wizard of the Kingdom");
    msgBox.setText(hint);
    msgBox.setStyleSheet(
        "QMessageBox { "
        "  background-color: #2C1810; "
        "  border: 3px solid #DAA520; "
        "  border-radius: 10px; "
        "} "
        "QLabel { "
        "  color: #F5E6D3; "
        "  font-family: 'Times New Roman'; "
        "  font-size: 16px; "
        "  font-style: italic; "
        "  padding: 10px; "
        "} "
        "QPushButton { "
        "  background-color: #8B4513; "
        "  color: #FFD700; "
        "  border: 2px solid #DAA520; "
        "  border-radius: 5px; "
        "  padding: 5px 15px; "
        "  font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "  background-color: #A0522D; "
        "}"
    );
    msgBox.exec();
}

void MainWindow::updatePlayerInventories()
{
    if (!m_game) return;

    const Player& p1 = m_game->getPlayer1();
    const Player& p2 = m_game->getPlayer2();

    std::cout << "[UI Update] P1 @" << &p1 << " (" << p1.getName() << ") has " << p1.getWonders().size() << " wonders.\n";
    std::cout << "[UI Update] P2 @" << &p2 << " (" << p2.getName() << ") has " << p2.getWonders().size() << " wonders.\n";

    if (m_lblProgressP1) {
        QString name = QString::fromStdString(p1.getName());
        if (name.length() > 8) name = name.left(6) + "..";
        m_lblProgressP1->setText(name);
    }
    if (m_lblProgressP2) {
        QString name = QString::fromStdString(p2.getName());
        if (name.length() > 8) name = name.left(6) + "..";
        m_lblProgressP2->setText(name);
    }

    auto calculateVP = [](const Player& p, const Player& opp) -> int {
        int vp = 0;
        vp += p.getVPFromMilitaryTokens();
        vp += p.getVPFromBlueCards(); 
        vp += p.getVPFromGuilds(opp);
        
        for (const auto& w : p.getWonders()) {
            if (w->getIsBuilt()) {
            }
        }
        
        for (const auto& [type, val] : p.getPoints()) {
            if (type == Points::Victory) vp += val;
        }
        
        return vp;
    };

    ui->playerDashboard->updateDashboard(
        p1.getName(), 
        p1.getCoins(), 
        p1.getResources(),
        p1.getWonders(),
        p1.getInventory(),
        calculateVP(p1, p2),
        p1.getProgressTokens() 
    );

    ui->opponentDashboard->updateDashboard(
        p2.getName(), 
        p2.getCoins(), 
        p2.getResources(),
        p2.getWonders(),
        p2.getInventory(),
        calculateVP(p2, p1),
        p2.getProgressTokens()
    );

    ui->militaryTrackWidget->updatePawnPosition(m_game->getBoard().getMilitaryTrack().getPawnPosition());

    auto clearLayout = [](QLayout* layout) {
        if (!layout) return;
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (item->widget()) delete item->widget();
            delete item;
        }
    };

    clearLayout(m_p1ProgressLayout);
    clearLayout(m_p2ProgressLayout);

    
    auto addPlayerTokenSlots = [this](const Player& p, QVBoxLayout* parentLayout) {
        if (!parentLayout) return;
        
        QWidget* slotContainer = new QWidget();
        slotContainer->setFixedWidth(50);
        slotContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);

        QGridLayout* slotLayout = new QGridLayout(slotContainer);
        slotLayout->setContentsMargins(0, 0, 0, 0);
        slotLayout->setSpacing(5);
        
        const auto& ownedTokens = p.getProgressTokens();
        
        auto getTokenSymbol = [](const QString& name) -> QString {
            QString n = name.toUpper().trimmed();
            if (n == "AGRICULTURE") return "Ag";
            if (n == "ARCHITECTURE") return "Ar";
            if (n == "ECONOMY") return "Ec";
            if (n == "LAW") return "La";
            if (n == "MASONRY") return "Ms";
            if (n == "MATHEMATICS") return "Mt";
            if (n == "PHILOSOPHY") return "Ph";
            if (n == "STRATEGY") return "St";
            if (n == "THEOLOGY") return "Th";
            if (n == "URBANISM") return "Ur";
            return "??";
        };

        for (int i = 0; i < 5; ++i) {
            QLabel* tokenLabel = new QLabel(this);
            tokenLabel->setFixedSize(36, 36); 
            tokenLabel->setAlignment(Qt::AlignCenter);
            
            if (i < (int)ownedTokens.size()) {
                QString name = QString::fromStdString(ownedTokens[i]->getName());
                tokenLabel->setText(getTokenSymbol(name));
                tokenLabel->setStyleSheet(
                    "QLabel { "
                    "  background: qradialgradient(cx:0.5, cy:0.5, radius: 0.5, fx:0.5, fy:0.5, stop:0 #A5D6A7, stop:1 #1B5E20); "
                    "  color: white; "
                    "  border: 2px solid #2E7D32; "
                    "  border-radius: 18px; " 
                    "  font-family: 'Arial', sans-serif; "
                    "  font-size: 14px; font-weight: bold;"
                    "}"
                );
                tokenLabel->setToolTip(name + ": " + getTokenDescription(name));
            } else {
                tokenLabel->setStyleSheet(
                    "QLabel { "
                    "  background: transparent; "
                    "  border: 2px dashed #5D4037; "
                    "  border-radius: 18px; "
                    "}"
                );
            }
            
            slotLayout->addWidget(tokenLabel, i, 0, Qt::AlignCenter); 
        }
        
        slotContainer->setLayout(slotLayout);
        parentLayout->addWidget(slotContainer, 0, Qt::AlignHCenter);
    };

    addPlayerTokenSlots(p1, m_p1ProgressLayout);
    addPlayerTokenSlots(p2, m_p2ProgressLayout);
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
    
    int numRows = static_cast<int>(rows.size());
    int availableHeight = ui->boardWidgetPage->height();
    if (availableHeight == 0) availableHeight = 600;
    
    double estimatedHeight = (numRows - 1) * 60 + 140 + 60;
    
    double scale = 1.0;
    if (estimatedHeight > availableHeight) {
        scale = (double)availableHeight / estimatedHeight;
        if (scale < 0.6) scale = 0.6; 
    }
    
    if (m_game->getCurrentAge() == 3) {
         if (scale > 0.8) scale = 0.8;
    }

    ui->boardWidgetPage->setScalingFactor(scale);
    
        auto getSymbolEmoji = [](const QString& symbolName) -> QString {
            QString lowerSymbolName = symbolName.toLower();
            if (lowerSymbolName == "moon") return "🌙";
            if (lowerSymbolName == "sun") return "☀️";
            if (lowerSymbolName == "mask") return "🎭";
            if (lowerSymbolName == "column") return "🏛️";
            if (lowerSymbolName == "droplet") return "💧";
            if (lowerSymbolName == "temple") return "⛩️";
            if (lowerSymbolName == "book") return "📖";
            if (lowerSymbolName == "gear") return "⚙️";
            if (lowerSymbolName == "lyre") return "🪕";
            if (lowerSymbolName == "pot") return "🏺";
            if (lowerSymbolName == "horse") return "🐎";
            if (lowerSymbolName == "sword") return "⚔️";
            if (lowerSymbolName == "castle") return "🏰";
            if (lowerSymbolName == "target") return "🎯";
            if (lowerSymbolName == "helmet") return "🪖";
            if (lowerSymbolName == "vase") return "🏺";
            if (lowerSymbolName == "barrel") return "🛢️";
            return "";
        };
    
        auto formatCost = [&](const std::map<Resource, uint8_t>& costMap, std::optional<Symbol> symbol) -> QString {
            QStringList parts;
            if (symbol.has_value()) {
                parts << getSymbolEmoji(QString::fromStdString(to_string(symbol.value())));
            }
    
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
                QString cost = formatCost(cardPtr->getCost(), cardPtr->getSymbol());
                QString effect = QString::fromStdString(cardPtr->getEffectDescription());
                QString unlocks_string;
                if(cardPtr->getUnlocks().has_value()){
                    unlocks_string = QString::fromStdString(to_string(cardPtr->getUnlocks().value()));
                }
                ui->boardWidgetPage->placeCard(id, name, getColorHex(col), isFaceUp, static_cast<int>(r), static_cast<int>(c), cardsInRow, m_game->getCurrentAge(), cost, effect, unlocks_string);
            }
        }}