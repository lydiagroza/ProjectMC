#include "MainWindow.h"
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <cmath>
#include <map>

// --- CONSTANTE DIMENSIUNI ---
const int CARD_W = 90;
const int CARD_H = 130;
const int SPACING = 15;
const int OVERLAP = 50;

// --- FUNCȚIE AJUTĂTOARE PENTRU UMBRE ---
void addShadow(QWidget* widget, int radius = 10, int offset = 3) {
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(radius);
    shadow->setOffset(offset, offset);
    shadow->setColor(QColor(0, 0, 0, 120));
    widget->setGraphicsEffect(shadow);
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    // 1. Setări Fereastră
    this->setWindowTitle("7 Wonders Duel");
    this->resize(1280, 900); // Putin mai inalta pentru inventare

    // 2. Widget Central
    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    // --- STIL INITIAL (AL TAU) ---
    centralWidget->setStyleSheet("background-color: rgb(52, 73, 94);");

    // --- TITLU ---
    titleLabel = new QLabel("7 WONDERS DUEL", centralWidget);
    titleLabel->setStyleSheet("color: rgb(255, 255, 255); font-size: 48px; font-weight: bold; background: transparent;");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setGeometry(0, 20, 1000, 80);
    addShadow(titleLabel); // Adaugam umbra si la titlu ca da bine

    // --- SUBTITLU ERA ---
    ageLabel = new QLabel("", centralWidget);
    ageLabel->setStyleSheet("color: rgb(241, 196, 15); font-size: 32px; font-weight: bold; background: transparent;");
    ageLabel->setAlignment(Qt::AlignCenter);
    ageLabel->setGeometry(0, 100, 1000, 50);
    addShadow(ageLabel);

    // --- BUTON START ---
    startButton = new QPushButton("START GAME", centralWidget);
    startButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgb(39, 174, 96); color: rgb(255, 255, 255);"
        "  font-size: 24px; font-weight: bold;"
        "  border-radius: 15px; border: 3px solid rgb(33, 145, 80);"
        "}"
        "QPushButton:hover { background-color: rgb(46, 204, 113); }"
    );
    int btnW = 250, btnH = 80;
    startButton->setGeometry((1000 - btnW) / 2, (800 - btnH) / 2, btnW, btnH);
    addShadow(startButton);

    connect(startButton, &QPushButton::clicked, this, &MainWindow::handleStartButton);

    // --- BUTON DEBUG: NEXT AGE ---
    nextAgeButton = new QPushButton("DEBUG: Next Age >>", centralWidget);
    nextAgeButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgb(231, 76, 60); color: rgb(255, 255, 255);"
        "  font-weight: bold; border-radius: 5px;"
        "}"
    );
    nextAgeButton->setGeometry(20, 850, 150, 40); // Mai jos
    nextAgeButton->hide();

    connect(nextAgeButton, &QPushButton::clicked, this, &MainWindow::handleNextAgeButton);

    // --- INIȚIALIZĂM ZONELE ---
    setupRightPanel();
    setupPlayerZones(); // <--- LOGICA NOUA: Inventare
}

MainWindow::~MainWindow() {
    clearPyramidUI();
}

// ---------------------------------------------------------
// LOGICA DE START ȘI SCHIMBARE ERĂ
// ---------------------------------------------------------
void MainWindow::handleStartButton() {
    startButton->hide();

    // Mutăm titlurile
    titleLabel->setFont(QFont("Arial", 24, QFont::Bold));
    titleLabel->setGeometry(0, 10, 1000, 40);
    ageLabel->setGeometry(0, 50, 1000, 40);

    nextAgeButton->show();

    m_game.getSetup().startAge(1);
    applyAgeStyle(1);

    drawPyramid();

    // Initializam inventarele (chiar daca sunt goale, sa apara zona)
    updatePlayerInventoryUI(m_game.getCurrentPlayer(), bottomPlayerZone);
    // Hack pt oponent (presupunem P1 vs P2)
    Player* opp = (m_game.getCurrentPlayer()->getName() == "Player 1") ? &m_game.getPlayer2() : &m_game.getPlayer1();
    updatePlayerInventoryUI(opp, topPlayerZone);
}

void MainWindow::applyAgeStyle(int age) {
    if (age == 1) {
        ageLabel->setText("AGE I");
        ageLabel->setStyleSheet("color: rgb(215, 204, 200); font-size: 32px; font-weight: bold;");
    }
    else if (age == 2) {
        ageLabel->setText("AGE II");
        ageLabel->setStyleSheet("color: rgb(174, 214, 241); font-size: 32px; font-weight: bold;");
    }
    else if (age == 3) {
        ageLabel->setText("AGE III");
        ageLabel->setStyleSheet("color: rgb(210, 180, 222); font-size: 32px; font-weight: bold;");
    }
}

void MainWindow::handleNextAgeButton() {
    static int debugCurrentAge = 1;
    if (debugCurrentAge < 3) {
        debugCurrentAge++;
        m_game.getSetup().startAge(debugCurrentAge);
        applyAgeStyle(debugCurrentAge);
        drawPyramid();
    }
    else {
        QMessageBox::information(this, "Info", "Ai ajuns la finalul jocului (Era 3)!");
    }
}

// ---------------------------------------------------------
// ZONA INVENTAR JUCĂTORI (SUS / JOS) - LOGICA NOUA
// ---------------------------------------------------------
void MainWindow::setupPlayerZones() {
    // Zona Sus (Oponent)
    topPlayerZone = new QWidget(centralWidget);
    topPlayerZone->setGeometry(100, 10, 800, 140);
    // Transparent sau fundal subtil
    topPlayerZone->setStyleSheet("background-color: rgba(0,0,0,40); border-radius: 10px;");

    QLabel* l1 = new QLabel("OPPONENT", topPlayerZone);
    l1->setObjectName("nameLabel");
    l1->setGeometry(10, 5, 400, 20);
    l1->setStyleSheet("color: rgb(200,200,200); font-weight: bold; background: transparent;");

    // Zona Jos (Player Curent)
    bottomPlayerZone = new QWidget(centralWidget);
    bottomPlayerZone->setGeometry(100, 750, 800, 140);
    bottomPlayerZone->setStyleSheet("background-color: rgba(0,0,0,40); border-radius: 10px;");

    QLabel* l2 = new QLabel("YOU", bottomPlayerZone);
    l2->setObjectName("nameLabel");
    l2->setGeometry(10, 5, 400, 20);
    l2->setStyleSheet("color: rgb(46, 204, 113); font-weight: bold; background: transparent;");
}

void MainWindow::updatePlayerInventoryUI(Player* player, QWidget* zone) {
    // 1. Curățăm cărțile vechi
    QList<QPushButton*> oldCards = zone->findChildren<QPushButton*>();
    for (auto c : oldCards) delete c;

    if (!player) return;

    // Actualizăm Textul (Nume + Bani)
    QLabel* nameLbl = zone->findChild<QLabel*>("nameLabel");
    if (nameLbl) {
        // Necesita: int getCoins() const in Player.h
        nameLbl->setText(QString("%1 | Coins: %2")
            .arg(QString::fromStdString(player->getName()))
            .arg(player->getCoins()));
    }

    // 2. Desenăm Cărțile grupate pe culori
    std::map<Color, int> colorPos = {
        {Color::Brown, 0}, {Color::Gray, 1}, {Color::Yellow, 2},
        {Color::Blue, 3}, {Color::Green, 4}, {Color::Red, 5}, {Color::Purple, 6}
    };

    int startX = 20;
    int startY = 30;
    int cardW = 60;
    int cardH = 80;
    int gapX = 80;
    int overlapY = 20;

    // Necesita: getter pt inventory in Player.h
    const auto& inventory = player->getInventory();

    for (auto const& [color, cards] : inventory) {
        int colIdx = colorPos[color];
        int currentX = startX + colIdx * gapX;

        // Culori specifice cartilor mici din inventar
        QString bgCol;
        if (color == Color::Brown) bgCol = "rgb(141, 110, 99)";
        else if (color == Color::Gray) bgCol = "rgb(149, 165, 166)";
        else if (color == Color::Yellow) bgCol = "rgb(241, 196, 15)";
        else if (color == Color::Blue) bgCol = "rgb(41, 128, 185)";
        else if (color == Color::Green) bgCol = "rgb(39, 174, 96)";
        else if (color == Color::Red) bgCol = "rgb(192, 57, 43)";
        else bgCol = "rgb(142, 68, 173)";

        for (size_t i = 0; i < cards.size(); ++i) {
            QPushButton* cardBtn = new QPushButton(zone);
            cardBtn->setGeometry(currentX, startY + (i * overlapY), cardW, cardH);

            cardBtn->setStyleSheet(QString(
                "QPushButton { background-color: %1; border: 1px solid black; border-radius: 4px; }"
                "QPushButton:hover { border: 1px solid white; margin-top: -5px; }"
            ).arg(bgCol));

            cardBtn->setToolTip(QString::fromStdString(cards[i]->getName()));
            cardBtn->show();
        }
    }
}

// ---------------------------------------------------------
// LOGICA DE DESENARE A PIRAMIDEI
// ---------------------------------------------------------
void MainWindow::drawPyramid() {
    clearPyramidUI();
    Board& board = m_game.getBoard();

    int currentAge = 1;
    if (ageLabel->text() == "AGE II") currentAge = 2;
    if (ageLabel->text() == "AGE III") currentAge = 3;

    QString themeColor;
    if (currentAge == 1)      themeColor = "rgb(141, 110, 99)";  // Maro
    else if (currentAge == 2) themeColor = "rgb(41, 128, 185)";  // Albastru
    else                      themeColor = "rgb(142, 68, 173)";  // Mov

    int centerX = 1000 / 2;
    int startY = 150;

    for (int r = 0; r < 20; ++r) {
        std::vector<CardNode*> rowNodes;
        for (int c = 0; c < 20; ++c) {
            CardNode* node = board.getNodeAt(r, c);
            if (!node) break;
            rowNodes.push_back(node);
        }
        if (rowNodes.empty()) break;

        // Fix Age 3 (Split Row)
        bool isSplitRow = (currentAge == 3 && r == 3);
        int visualCount = isSplitRow ? 4 : rowNodes.size();
        int totalW = visualCount * CARD_W + (visualCount - 1) * SPACING;
        int currentX = centerX - (totalW / 2);
        int currentY = startY + (r * (CARD_H - OVERLAP));

        for (int c = 0; c < rowNodes.size(); ++c) {
            CardNode* node = rowNodes[c];

            if (isSplitRow) {
                if (c == 0) currentX += (CARD_W + SPACING) / 2;
                else if (c == 1) currentX += (CARD_W + SPACING);
            }

            if (!node->isPlayed()) {
                QPushButton* btn = new QPushButton(centralWidget);
                btn->setGeometry(currentX, currentY, CARD_W, CARD_H);

                // --- STOCARE DATE PENTRU CLICK ---
                btn->setProperty("row", r);
                btn->setProperty("col", c);

                // === STILIZARE VIZUALĂ (A TA) ===
                if (node->getFace() == Face::Down) {
                    // Spate
                    btn->setText("");
                    btn->setStyleSheet(QString(
                        "QPushButton {"
                        "  background-color: %1;"
                        "  border: 2px solid rgb(0, 0, 0);" // Contur Negru
                        "  border-radius: 8px;"
                        "}"
                    ).arg(themeColor));
                }
                else {
                    // Față
                    QString name = QString::fromStdString(node->getName());
                    if (name.length() > 10) name = name.left(8) + "..";
                    btn->setText(name);

                    if (node->isPlayable()) {
                        // --- HOVER LOGIC: CALCUL COST ---
                        Player* curr = m_game.getCurrentPlayer();
                        Player* opp = (curr->getName() == "Player 1") ? &m_game.getPlayer2() : &m_game.getPlayer1();

                        // Necesita functia getTotalCardCost in Player
                        int cost = curr->getTotalCardCost(*node->getCard(), *opp);
                        QString tip;
                        if (cost == -1) tip = QString("%1\nCost: Indisponibil").arg(QString::fromStdString(node->getName()));
                        else if (cost == 0) tip = QString("%1\nCost: GRATIS").arg(QString::fromStdString(node->getName()));
                        else tip = QString("%1\nCost Total: %2 Coins").arg(QString::fromStdString(node->getName())).arg(cost);

                        btn->setToolTip(tip);

                        // STIL DISPONIBIL (AL TAU: Alb cu contur Ridge)
                        btn->setStyleSheet(QString(
                            "QPushButton {"
                            "  background-color: rgb(255, 255, 255);"
                            "  color: rgb(0, 0, 0);"
                            "  font-weight: bold;"
                            "  border: 5px ridge %1;"
                            "  border-radius: 8px;"
                            "}"
                            "QPushButton:hover { background-color: rgb(236, 240, 241); }"
                        ).arg(themeColor));

                        // Conectare CLICK
                        connect(btn, &QPushButton::clicked, this, &MainWindow::onCardClicked);
                    }
                    else {
                        // STIL BLOCAT (AL TAU)
                        btn->setStyleSheet(QString(
                            "QPushButton {"
                            "  background-color: rgb(240, 240, 240);"
                            "  color: rgb(149, 165, 166);"
                            "  border: 2px dashed %1;"
                            "  border-radius: 8px;"
                            "}"
                        ).arg(themeColor));
                    }
                }
                btn->show();
                m_cardButtons.push_back(btn);
            }
            currentX += CARD_W + SPACING;
        }
    }
}

// ---------------------------------------------------------
// ACTIUNE CLICK (LOGICA DE JOC)
// ---------------------------------------------------------
void MainWindow::onCardClicked() {
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int r = btn->property("row").toInt();
    int c = btn->property("col").toInt();

    Board& board = m_game.getBoard();
    CardNode* node = board.getNodeAt(r, c);
    if (!node || !node->isPlayable()) return;

    Player* curr = m_game.getCurrentPlayer();
    Player* opp = (curr->getName() == "Player 1") ? &m_game.getPlayer2() : &m_game.getPlayer1();

    // Dialog Confirmare
    QMessageBox msg;
    msg.setWindowTitle("Actiune Joc");
    msg.setText(QString("Card: %1").arg(QString::fromStdString(node->getCard()->getName())));

    QAbstractButton* bBuy = msg.addButton("Construieste", QMessageBox::AcceptRole);
    QAbstractButton* bSell = msg.addButton("Vinde (+2 Coins)", QMessageBox::RejectRole);
    msg.addButton("Anuleaza", QMessageBox::DestructiveRole);

    msg.exec();

    bool actionDone = false;

    if (msg.clickedButton() == bBuy) {
        if (curr->buyCard(node->getCard(), *opp, board)) {
            actionDone = true;
        }
        else {
            QMessageBox::warning(this, "Eroare", "Fonduri Insuficiente!");
        }
    }
    else if (msg.clickedButton() == bSell) {
        curr->discardCard(*node->getCard());
        actionDone = true;
    }

    if (actionDone) {
        node->updatePlayedStatus(true);
        board.updateVisibility();

        // REDESENARE COMPLETA
        drawPyramid();
        updatePlayerInventoryUI(&m_game.getPlayer1(), bottomPlayerZone);
        updatePlayerInventoryUI(&m_game.getPlayer2(), topPlayerZone);
        updateMilitaryPawn();
        // m_game.switchTurn(); // Decomenteaza cand esti gata
    }
}

// ---------------------------------------------------------
// PANOUL DIN DREAPTA (Militar & Science) - STILUL TAU
// ---------------------------------------------------------
void MainWindow::setupRightPanel() {
    rightPanel = new QWidget(centralWidget);
    rightPanel->setGeometry(1280 - 280, 20, 260, 760);
    // Transparent
    rightPanel->setStyleSheet("background-color: transparent;");

    // COLOANA 1: CONFLICT
    int trackX = 30;
    int trackY = 60;
    int trackH = 640;
    int trackW = 50;

    QLabel* milLabel = new QLabel("CONFLICT", rightPanel);
    milLabel->setGeometry(trackX - 15, 20, 80, 30);
    milLabel->setAlignment(Qt::AlignCenter);
    milLabel->setStyleSheet("color: rgb(231, 76, 60); font-weight: bold; font-size: 14px; letter-spacing: 2px;");

    // Pista Vizuala (Gradientul Tau)
    QLabel* trackVisual = new QLabel(rightPanel);
    trackVisual->setGeometry(trackX, trackY, trackW, trackH);
    trackVisual->setStyleSheet(
        "border: 2px solid rgb(100, 100, 100);"
        "border-radius: 25px;"
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "  stop: 0.0 rgb(148, 49, 38),"   // Sus
        "  stop: 0.2 rgb(230, 126, 34),"
        "  stop: 0.4 rgb(241, 196, 15),"
        "  stop: 0.5 rgb(236, 240, 241)," // Alb
        "  stop: 0.6 rgb(241, 196, 15),"
        "  stop: 0.8 rgb(230, 126, 34),"
        "  stop: 1.0 rgb(148, 49, 38));"  // Jos
    );

    // Linii discrete
    int steps = 19;
    float stepH = (float)trackH / steps;
    for (int i = 1; i < steps; ++i) {
        if (i == 9) continue;
        QWidget* line = new QWidget(trackVisual);
        line->setGeometry(5, i * stepH, trackW - 10, 1);
        line->setStyleSheet("background-color: rgba(0,0,0, 50);");
    }
    QWidget* centerLine = new QWidget(trackVisual);
    centerLine->setGeometry(0, 9 * stepH - 1, trackW, 3);
    centerLine->setStyleSheet("background-color: rgba(0,0,0, 100);");

    // COLOANA 2: SCIENCE
    int tokenColumnX = 140;
    QLabel* tokenLabel = new QLabel("SCIENCE", rightPanel);
    tokenLabel->setGeometry(tokenColumnX - 20, 20, 90, 30);
    tokenLabel->setAlignment(Qt::AlignCenter);
    tokenLabel->setStyleSheet("color: rgb(46, 204, 113); font-weight: bold; font-size: 14px; letter-spacing: 2px;");

    int tokenSize = 60;
    int tokenGap = 30;
    int startTokensY = trackY + 50;
    QString symbols[] = { "⚖️", "🔭", "📐", "⚗️", "⚙️" };

    for (int i = 0; i < 5; ++i) {
        QPushButton* token = new QPushButton(rightPanel);
        int x = tokenColumnX;
        int y = startTokensY + i * (tokenSize + tokenGap);
        token->setGeometry(x, y, tokenSize, tokenSize);
        token->setText(symbols[i]);

        // Stilul Tau 3D
        token->setStyleSheet(
            "QPushButton {"
            "  background-color: qradialgradient(cx:0.4, cy:0.4, radius: 0.8, fx:0.4, fy:0.4,"
            "    stop:0 rgb(88, 214, 141), stop:1 rgb(25, 111, 61));"
            "  border: 2px solid rgb(241, 196, 15);"
            "  border-radius: 30px;"
            "  color: white; font-size: 24px;"
            "}"
            "QPushButton:hover { border: 2px solid white; }"
        );
        token->show();
        progressTokens.push_back(token);
    }

    // PIONUL (Scut)
    militaryPawn = new QPushButton(rightPanel);
    militaryPawn->setText("⚔️");
    militaryPawn->setStyleSheet(
        "QPushButton {"
        "  background: qradialgradient(cx:0.5, cy:0.5, radius: 0.8, fx:0.5, fy:0.5,"
        "    stop:0 rgb(231, 76, 60), stop:1 rgb(148, 49, 38));"
        "  border: 2px solid white;"
        "  border-radius: 20px;"
        "  font-size: 20px;"
        "}"
    );
    militaryPawn->setGeometry(0, 0, 40, 40);

    // Umbra Pion
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(10); shadow->setOffset(2, 2);
    militaryPawn->setGraphicsEffect(shadow);

    updateMilitaryPawn();
}

void MainWindow::updateMilitaryPawn() {
    int currentScore = 0;
    int visualIndex = 9 - currentScore;

    int trackYStart = 60;
    int trackHeight = 640;
    float stepH = (float)trackHeight / 19;

    int pawnY = trackYStart + (visualIndex * stepH);
    int pawnX = 35; // Centrat pe pista de 50 (X=30) -> 30+25=55. Pion 40 -> 55-20=35.

    pawnY -= 20 - (stepH / 2);
    pawnY -= 10;

    militaryPawn->move(pawnX, pawnY);
}