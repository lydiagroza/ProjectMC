#include "MainWindow.h"
#include <QApplication>
#include <QScreen>
#include <QMessageBox> // Necesar pentru pop-up la finalul jocului

// --- CONSTANTE DIMENSIUNI ---
const int CARD_W = 90;   // Lățime carte
const int CARD_H = 130;  // Înălțime carte
const int SPACING = 15;  // Spațiu orizontal între cărți
const int OVERLAP = 50;  // Suprapunere verticală

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    // 1. Setări Fereastră
    this->setWindowTitle("7 Wonders Duel");
    this->resize(1280, 800);

    // 2. Widget Central
    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    //fundal inital joc - roz dragut
    centralWidget->setStyleSheet("background-color: rgb(230, 179, 209);");

    // --- TITLU ---
    titleLabel = new QLabel("7 WONDERS DUEL", centralWidget);
    titleLabel->setStyleSheet("color: rgb(255, 255, 255); font-size: 48px; font-weight: bold; background: transparent;");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setGeometry(0, 50, 1280, 80);

    // --- SUBTITLU ERA (Initial gol) ---
    ageLabel = new QLabel("", centralWidget);
    // RGB(241, 196, 15) = Galben auriu
    ageLabel->setStyleSheet("color: rgb(241, 196, 15); font-size: 32px; font-weight: bold; background: transparent;");
    ageLabel->setAlignment(Qt::AlignCenter);
    ageLabel->setGeometry(0, 130, 1280, 50);

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

    // Centrare Buton
    int btnW = 250, btnH = 80;
    startButton->setGeometry((1280 - btnW) / 2, (800 - btnH) / 2, btnW, btnH);

    connect(startButton, &QPushButton::clicked, this, &MainWindow::handleStartButton);

    // --- BUTON DEBUG: NEXT AGE ---
    nextAgeButton = new QPushButton("DEBUG: Next Age >>", centralWidget);
    nextAgeButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgb(231, 76, 60); color: rgb(255, 255, 255);" // Roșu
        "  font-weight: bold; border-radius: 5px;"
        "}"
    );
    // Îl punem jos în dreapta
    nextAgeButton->setGeometry(1280 - 170, 800 - 60, 150, 40);
    // Îl ascundem inițial, îl arătăm doar după Start
    nextAgeButton->hide();

    connect(nextAgeButton, &QPushButton::clicked, this, &MainWindow::handleNextAgeButton);
}

MainWindow::~MainWindow() {
    clearPyramidUI();
}

// Funcție pentru a schimba textele ajutătoare în funcție de eră
void MainWindow::applyAgeStyle(int age) {
    if (age == 1) {
        ageLabel->setText("AGE I");
        // RGB(215, 204, 200) = Bej deschis
        ageLabel->setStyleSheet("color: rgb(215, 204, 200); font-size: 32px; font-weight: bold;");
    }
    else if (age == 2) {
        ageLabel->setText("AGE II");
        // RGB(174, 214, 241) = Albastru deschis
        ageLabel->setStyleSheet("color: rgb(174, 214, 241); font-size: 32px; font-weight: bold;");
    }
    else if (age == 3) {
        ageLabel->setText("AGE III");
        // RGB(210, 180, 222) = Mov deschis
        ageLabel->setStyleSheet("color: rgb(210, 180, 222); font-size: 32px; font-weight: bold;");
    }
}

void MainWindow::handleStartButton() {
    // 1. Ascundem meniul de start
    startButton->hide();

    // 2. Mutăm titlurile mai sus pentru a face loc piramidei
    titleLabel->setFont(QFont("Arial", 24, QFont::Bold));
    titleLabel->setGeometry(0, 10, 1280, 40);
    ageLabel->setGeometry(0, 50, 1280, 40);

    // 3. Arătăm butonul de debug
    nextAgeButton->show();

    // 4. Inițializăm Backend-ul (Era 1)
    m_game.getSetup().startAge(1);

    // 5. Setăm stilul vizual
    applyAgeStyle(1);

    // 6. Desenăm Piramida
    drawPyramid();
}

void MainWindow::clearPyramidUI() {
    for (QPushButton* btn : m_cardButtons) {
        delete btn;
    }
    m_cardButtons.clear();
}

void MainWindow::drawPyramid() {
    clearPyramidUI();
    Board& board = m_game.getBoard();

    // 1. Aflăm Era Curentă
    int currentAge = 1;
    if (ageLabel->text() == "AGE II") currentAge = 2;
    if (ageLabel->text() == "AGE III") currentAge = 3;

    // 2. Alegem Culoarea Tematică (RGB String direct)
    QString themeColor;
    if (currentAge == 1)      themeColor = "rgb(120, 78, 23)";  // Maro (Age 1)
    else if (currentAge == 2) themeColor = "rgb(93, 188, 207)";  // Albastru (Age 2)
    else                      themeColor = "rgb(134, 73, 171)";  // Mov (Age 3)

    int centerX = this->width() / 2;
    int startY = 150;

    // Parcurgem rândurile
    for (int r = 0; r < 20; ++r) {
        std::vector<CardNode*> rowNodes;
        for (int c = 0; c < 20; ++c) {
            CardNode* node = board.getNodeAt(r, c);
            if (!node) break;
            rowNodes.push_back(node); 
        }
        if (rowNodes.empty()) break;

        // --- FIX PENTRU POZIȚIONARE (AGE 3 - RÂNDUL DIN MIJLOC) ---
        bool isSplitRow = (currentAge == 3 && r == 3);
        int visualCount = isSplitRow ? 4 : rowNodes.size();

        int totalW = visualCount * CARD_W + (visualCount - 1) * SPACING;
        int currentX = centerX - (totalW / 2);
        int currentY = startY + (r * (CARD_H - OVERLAP));

        for (int c = 0; c < rowNodes.size(); ++c) {
            CardNode* node = rowNodes[c];

            // TRUC DE ALINIERE PENTRU "RÂNDUL SPART":
            if (isSplitRow) {
                if (c == 0) {
                    currentX += (CARD_W + SPACING) / 2;
                }
                else if (c == 1) {
                    currentX += (CARD_W + SPACING);
                }
            }

            if (!node->isPlayed()) {
                QPushButton* btn = new QPushButton(centralWidget);
                btn->setGeometry(currentX, currentY, CARD_W, CARD_H);

                // === LOGICA VIZUALĂ CU RGB ===

                if (node->getFace() == Face::Down) {
                    // CAZ 1: SPATELE CĂRȚII
                    btn->setText("");
                    btn->setStyleSheet(QString(
                        "QPushButton {"
                        "  background-color: %1;" // Maro/Albastru/Mov RGB
                        "  border: 2px solid rgb(0, 0, 0);" // Contur Negru RGB(0,0,0)
                        "  border-radius: 8px;"
                        "}"
                    ).arg(themeColor));
                }
                else {
                    // CAZ 2: FAȚA CĂRȚII
                    QString name = QString::fromStdString(node->getName());
                    if (name.length() > 10) name = name.left(8) + "..";
                    btn->setText(name);

                    if (node->isPlayable()) {
                        // DISPONIBILĂ
                        // Contur RIDGE (3D) în culoarea Erei
                        btn->setStyleSheet(QString(
                            "QPushButton {"
                            "  background-color: rgb(255, 255, 255);" // Alb RGB
                            "  color: rgb(0, 0, 0);" // Negru RGB
                            "  font-weight: bold;"
                            "  border: 5px ridge %1;" // Contur 3D cu culoarea erei
                            "  border-radius: 8px;"
                            "}"
                            "QPushButton:hover { background-color: rgb(236, 240, 241); }" // Hover gri deschis RGB
                        ).arg(themeColor));

                        connect(btn, &QPushButton::clicked, this, &MainWindow::onCardClicked);
                    }
                    else {
                        // BLOCATĂ
                        // Contur PUNCTAT
                        btn->setStyleSheet(QString(
                            "QPushButton {"
                            "  background-color: rgb(240, 240, 240);" // Gri foarte deschis RGB
                            "  color: rgb(149, 165, 166);" // Gri text RGB
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

void MainWindow::onCardClicked() {
    QMessageBox::information(this, "Click", "Ai dat click pe o carte!");
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