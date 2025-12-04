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
    this->setWindowTitle("7 Wonders Duel - UI DEMO");
    this->resize(1280, 900);

    // 2. Widget Central
    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    // --- STIL INITIAL ---
    centralWidget->setStyleSheet("background-color: rgb(52, 73, 94);");

    // --- TITLU ---
    titleLabel = new QLabel("7 WONDERS DUEL", centralWidget);
    titleLabel->setStyleSheet("color: rgb(255, 255, 255); font-size: 48px; font-weight: bold; background: transparent;");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setGeometry(0, 20, 1000, 80);
    addShadow(titleLabel);

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

    // --- BUTON DEBUG ---
    nextAgeButton = new QPushButton("DEBUG: Next Age >>", centralWidget);
    nextAgeButton->setStyleSheet("background-color: rgb(231, 76, 60); color: white; border-radius: 5px; font-weight: bold;");
    nextAgeButton->setGeometry(20, 850, 150, 40);
    nextAgeButton->hide();
    connect(nextAgeButton, &QPushButton::clicked, this, &MainWindow::handleNextAgeButton);

    // --- INIȚIALIZĂM ZONELE VIZUALE ---
    setupRightPanel();
    setupPlayerZones();
}

MainWindow::~MainWindow() {
    clearPyramidUI();
}

// ---------------------------------------------------------
// LOGICA DE START (SIMULATĂ)
// ---------------------------------------------------------
void MainWindow::handleStartButton() {
    startButton->hide();

    // Mutăm elementele de titlu
    titleLabel->setFont(QFont("Arial", 24, QFont::Bold));
    titleLabel->setGeometry(0, 10, 1000, 40);
    ageLabel->setGeometry(0, 50, 1000, 40);
    nextAgeButton->show();

    // AICI PORNEAM LOGICA REALA. ACUM DOAR SIMULĂM VISUALUL.
    // m_game.getSetup().startAge(1); <--- COMENTAT PENTRU SIGURANȚĂ

    applyAgeStyle(1);

    // Desenăm o piramidă "falsă" sau cea reală dacă structura e încărcată, 
    // dar fără interacțiune complexă.
    // Incercam sa desenam structura reala daca m_game nu crapa la startAge.
    // Daca crapa, comentati linia startAge si drawPyramid va fi gol.
    try {
        m_game.getSetup().startAge(1);
        drawPyramid();
    }
    catch (...) {
        // Daca nu merge backend-ul, nu desenam piramida, dar restul UI-ului apare.
    }

    // DESENĂM INVENTARE "FALSE" (MOCK) CA SĂ VEZI CUM ARATĂ
    // Trimitem nullptr pentru că funcția noastră modificată de inventar va ignora playerul
    // și va desena date false.
    updatePlayerInventoryUI(nullptr, bottomPlayerZone);
    updatePlayerInventoryUI(nullptr, topPlayerZone);
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
    else {
        ageLabel->setText("AGE III");
        ageLabel->setStyleSheet("color: rgb(210, 180, 222); font-size: 32px; font-weight: bold;");
    }
}

void MainWindow::handleNextAgeButton() {
    // Schimbare vizuală simplă
    static int debugAge = 1;
    debugAge++;
    if (debugAge > 3) debugAge = 1;
    applyAgeStyle(debugAge);

    // Putem forța redesenarea piramidei reale dacă backend-ul permite
    try {
        m_game.getSetup().startAge(debugAge);
        drawPyramid();
    }
    catch (...) {}
}

// ---------------------------------------------------------
// ZONA INVENTAR (MOCK / FAKE DATA)
// ---------------------------------------------------------
void MainWindow::setupPlayerZones() {
    // Zona Sus (Oponent)
    topPlayerZone = new QWidget(centralWidget);
    topPlayerZone->setGeometry(100, 10, 800, 140);
    topPlayerZone->setStyleSheet("background-color: rgba(0,0,0,40); border-radius: 10px;");

    QLabel* l1 = new QLabel("OPPONENT", topPlayerZone);
    l1->setObjectName("nameLabel"); // Îl marcam ca să-l găsim
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

// *** AICI E TRUCUL: IGNORĂM DATA REALA ȘI DESENĂM CEVA FIX ***
void MainWindow::updatePlayerInventoryUI(Player* /*unused*/, QWidget* zone) {
    // 1. Curățăm
    QList<QPushButton*> oldCards = zone->findChildren<QPushButton*>();
    for (auto c : oldCards) delete c;

    // 2. Setăm Text Fals (Mock)
    QLabel* nameLbl = zone->findChild<QLabel*>("nameLabel");
    if (nameLbl) {
        // Dacă e zona de jos (YOU), punem banii tăi fictivi
        if (zone == bottomPlayerZone)
            nameLbl->setText("YOU | Coins: 7 (Demo)");
        else
            nameLbl->setText("OPPONENT | Coins: 5 (Demo)");
    }

    // 3. Generăm cărți false ca să vezi layout-ul
    // Culori: Maro, Gri, Galben, Albastru, Verde, Rosu, Mov
    QString colors[] = {
        "rgb(141, 110, 99)", "rgb(149, 165, 166)", "rgb(241, 196, 15)",
        "rgb(41, 128, 185)", "rgb(39, 174, 96)", "rgb(192, 57, 43)", "rgb(142, 68, 173)"
    };

    int startX = 20;
    int startY = 30;
    int cardW = 60;
    int cardH = 80;
    int gapX = 80;
    int overlapY = 20;

    // Pentru fiecare culoare, desenăm 2 cărți fictive
    for (int colIdx = 0; colIdx < 7; ++colIdx) {
        int currentX = startX + colIdx * gapX;
        QString bgCol = colors[colIdx];

        // Facem 2 cărți suprapuse
        for (int i = 0; i < 2; ++i) {
            QPushButton* cardBtn = new QPushButton(zone);
            cardBtn->setGeometry(currentX, startY + (i * overlapY), cardW, cardH);

            cardBtn->setStyleSheet(QString(
                "QPushButton { background-color: %1; border: 1px solid black; border-radius: 4px; }"
                "QPushButton:hover { border: 1px solid white; margin-top: -5px; }"
            ).arg(bgCol));

            cardBtn->setToolTip("Carte Demo");
            cardBtn->show();
        }
    }
}

// ---------------------------------------------------------
// PIRAMIDA (Păstrăm logica de poziționare, scoatem logica de bani)
// ---------------------------------------------------------
void MainWindow::drawPyramid() {
    clearPyramidUI();
    Board& board = m_game.getBoard();

    // Culori vizuale pentru piramida
    int currentAge = 1; // Putem lua din label
    if (ageLabel->text().contains("II")) currentAge = 2;
    if (ageLabel->text().contains("III")) currentAge = 3;

    QString themeColor = "rgb(120, 78, 23)"; // Maro default
    if (currentAge == 2) themeColor = "rgb(93, 188, 207)";
    if (currentAge == 3) themeColor = "rgb(134, 73, 171)";

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

                // Salvăm doar coordonatele, nu ne pasa de logica acum
                btn->setProperty("row", r);
                btn->setProperty("col", c);

                if (node->getFace() == Face::Down) {
                    btn->setText("");
                    btn->setStyleSheet(QString(
                        "QPushButton { background-color: %1; border: 2px solid rgb(0,0,0); border-radius: 8px; }"
                    ).arg(themeColor));
                }
                else {
                    // Afisam numele daca putem, altfel "Card"
                    QString name = "Card";
                    try { name = QString::fromStdString(node->getName()); }
                    catch (...) {}
                    if (name.length() > 10) name = name.left(8) + "..";
                    btn->setText(name);

                    if (node->isPlayable()) {
                        // Stil Activ
                        btn->setStyleSheet(QString(
                            "QPushButton { background-color: rgb(255,255,255); color: black; font-weight: bold; border: 5px ridge %1; border-radius: 8px; }"
                            "QPushButton:hover { background-color: rgb(236,240,241); }"
                        ).arg(themeColor));

                        // FARA LOGICA DE COST (Eliminam erorile)
                        btn->setToolTip("Cost: ??? (Demo)");
                        connect(btn, &QPushButton::clicked, this, &MainWindow::onCardClicked);
                    }
                    else {
                        // Stil Blocat
                        btn->setStyleSheet(QString(
                            "QPushButton { background-color: rgb(240,240,240); color: gray; border: 2px dashed %1; border-radius: 8px; }"
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
// CLICK SIMPLIFICAT (DOAR MESAJ VIZUAL)
// ---------------------------------------------------------
void MainWindow::onCardClicked() {
    // Doar un mesaj sa stim ca merge click-ul
    QMessageBox::information(this, "UI Demo",
        "Ai dat click pe o carte!\n\n"
        "Logica de cumparare este dezactivata in acest demo vizual\n"
        "pentru a evita erorile de compilare.");
}

// ---------------------------------------------------------
// PANOUL DREAPTA (VISUAL ONLY)
// ---------------------------------------------------------
void MainWindow::setupRightPanel() {
    rightPanel = new QWidget(centralWidget);
    rightPanel->setGeometry(1280 - 280, 20, 260, 760);
    rightPanel->setStyleSheet("background-color: transparent;");

    // Conflict Track
    int trackX = 30; int trackY = 60; int trackH = 640; int trackW = 50;
    QLabel* milLabel = new QLabel("CONFLICT", rightPanel);
    milLabel->setGeometry(trackX - 15, 20, 80, 30);
    milLabel->setAlignment(Qt::AlignCenter);
    milLabel->setStyleSheet("color: rgb(231, 76, 60); font-weight: bold;");

    QLabel* trackVisual = new QLabel(rightPanel);
    trackVisual->setGeometry(trackX, trackY, trackW, trackH);
    trackVisual->setStyleSheet(
        "border: 2px solid rgb(100,100,100); border-radius: 25px;"
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "  stop: 0 rgb(148,49,38), stop: 0.4 rgb(241,196,15), stop: 0.5 white, stop: 0.6 rgb(241,196,15), stop: 1 rgb(148,49,38));"
    );

    // Linii
    int steps = 19; float stepH = (float)trackH / steps;
    for (int i = 1; i < steps; ++i) {
        if (i == 9) continue;
        QWidget* line = new QWidget(trackVisual);
        line->setGeometry(5, i * stepH, trackW - 10, 1);
        line->setStyleSheet("background-color: rgba(0,0,0,50);");
    }
    QWidget* cLine = new QWidget(trackVisual);
    cLine->setGeometry(0, 9 * stepH - 1, trackW, 3);
    cLine->setStyleSheet("background-color: rgba(0,0,0,100);");

    // Tokens
    int tX = 140;
    QLabel* tLabel = new QLabel("SCIENCE", rightPanel);
    tLabel->setGeometry(tX - 20, 20, 90, 30);
    tLabel->setAlignment(Qt::AlignCenter);
    tLabel->setStyleSheet("color: rgb(46, 204, 113); font-weight: bold;");

    int tSize = 60; int gap = 30; int startTY = trackY + 50;
    QString sym[] = { "⚖️", "🔭", "📐", "⚗️", "⚙️" };
    for (int i = 0; i < 5; ++i) {
        QPushButton* t = new QPushButton(rightPanel);
        t->setGeometry(tX, startTY + i * (tSize + gap), tSize, tSize);
        t->setText(sym[i]);
        t->setStyleSheet("QPushButton { background-color: qradialgradient(cx:0.4, cy:0.4, radius:0.8, fx:0.4, fy:0.4, stop:0 rgb(88,214,141), stop:1 rgb(25,111,61)); border: 2px solid rgb(241,196,15); border-radius: 30px; color: white; font-size: 24px; }");
        t->show();
        progressTokens.push_back(t);
    }

    // Pion
    militaryPawn = new QPushButton(rightPanel);
    militaryPawn->setText("⚔️");
    militaryPawn->setStyleSheet("background: qradialgradient(cx:0.5, cy:0.5, radius:0.8, fx:0.5, fy:0.5, stop:0 rgb(231,76,60), stop:1 rgb(148,49,38)); border: 2px solid white; border-radius: 20px; font-size: 20px;");
    militaryPawn->setGeometry(0, 0, 40, 40);
    addShadow(militaryPawn, 5, 2);

    updateMilitaryPawn();
}

void MainWindow::updateMilitaryPawn() {
    int score = 0; // Hardcodat pt Demo
    int vIndex = 9 - score;
    int startY = 60; int h = 640; float step = (float)h / 19;

    int y = startY + vIndex * step - 20 + step / 2 - 10;
    int x = 35;
    militaryPawn->move(x, y);
}
void MainWindow::clearPyramidUI() {}
