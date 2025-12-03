#include "MainWindow.h"
#include <QString>
#include <QMessageBox> // Pentru mesaje pop-up

// Constante pentru dimensiuni
const int CARD_W = 90;
const int CARD_H = 130;
const int SPACING = 15;
const int OVERLAP = 50;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    // 1. Setup Window
    this->setWindowTitle("7 Wonders Duel - Qt Interface");
    this->resize(1024, 768); // Facem fereastra mai mare pentru piramida

    // 2. Setup UI Elements
    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    // Folosim un layout doar pentru titlu si butonul de start
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setAlignment(Qt::AlignTop); // Le punem sus

    infoLabel = new QLabel("Welcome to 7 Wonders Duel!", centralWidget);
    QFont font = infoLabel->font();
    font.setPointSize(16);
    font.setBold(true);
    infoLabel->setFont(font);
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setStyleSheet("color: #2c3e50; background-color: #ecf0f1; padding: 10px; border-radius: 5px;");

    startButton = new QPushButton("Start Game", centralWidget);
    startButton->setMinimumHeight(50);
    startButton->setStyleSheet("background-color: #27ae60; color: white; font-weight: bold; font-size: 14px; border-radius: 5px;");

    // Adaugam in layout
    mainLayout->addWidget(infoLabel);
    mainLayout->addWidget(startButton);

    // Conectam butonul
    connect(startButton, &QPushButton::clicked, this, &MainWindow::handleStartButton);

    // 3. Initialize Game (Backend)
    // Putem lasa initializarea aici sau o mutam in Start
    // m_game.initialize(); 
}

MainWindow::~MainWindow() {
    clearPyramidUI();
}

void MainWindow::handleStartButton() {
    // 1. Actualizam textul
    infoLabel->setText("Age I - Pyramid");

    // 2. Ascundem butonul de start (ca sa nu il apesi de 2 ori)
    startButton->hide();
    // Sau poti sa il stergi din layout: mainLayout->removeWidget(startButton); startButton->deleteLater();

    // 3. Pornim logica jocului pentru Era 1
    m_game.getSetup().startAge(1);

    // 4. Desenam Piramida
    drawPyramid();
}

void MainWindow::clearPyramidUI() {
    for (QPushButton* btn : m_cardButtons) {
        delete btn;
    }
    m_cardButtons.clear();
}

void MainWindow::drawPyramid() {
    clearPyramidUI(); // Curatam inainte sa desenam

    Board& board = m_game.getBoard();

    // Centrul ecranului
    int centerX = this->width() / 2;
    int startY = 150; // Lasam loc sus pentru Label

    // Parcurgem randurile (max 20)
    for (int r = 0; r < 20; ++r) {

        std::vector<CardNode*> rowNodes;
        for (int c = 0; c < 20; ++c) {
            CardNode* node = board.getNodeAt(r, c);
            if (node == nullptr) break;
            rowNodes.push_back(node);
        }

        if (rowNodes.empty()) break; // Gata piramida

        // Calcule pentru centrare
        int totalRowWidth = rowNodes.size() * CARD_W + (rowNodes.size() - 1) * SPACING;
        int currentX = centerX - (totalRowWidth / 2);
        int currentY = startY + (r * (CARD_H - OVERLAP));

        for (int c = 0; c < rowNodes.size(); ++c) {
            CardNode* node = rowNodes[c];

            // Daca nu e jucata (e pe masa)
            if (!node->isPlayed()) {
                QPushButton* btn = new QPushButton(centralWidget);
                btn->setGeometry(currentX, currentY, CARD_W, CARD_H);

                // --- STILIZARE ---
                if (node->getFace() == Face::Down) {
                    btn->setText("???");
                    btn->setStyleSheet(
                        "QPushButton { background-color: #95a5a6; color: white; border: 1px solid #7f8c8d; border-radius: 8px; }"
                    );
                }
                else {
                    QString name = QString::fromStdString(node->getName());
                    btn->setText(name);

                    if (node->isPlayable()) {
                        // Carte disponibila (Verde/Albastru)
                        btn->setStyleSheet(
                            "QPushButton { background-color: #3498db; color: white; font-weight: bold; border: 2px solid #2980b9; border-radius: 8px; }"
                            "QPushButton:hover { background-color: #5dade2; }"
                        );
                        // Aici vom conecta click-ul mai tarziu
                        // connect(btn, &QPushButton::clicked, this, &MainWindow::onCardClicked);
                    }
                    else {
                        // Carte vizibila dar blocata (Galbena/Gri deschis)
                        btn->setStyleSheet(
                            "QPushButton { background-color: #f1c40f; color: #7f8c8d; border: 1px dashed #f39c12; border-radius: 8px; }"
                        );
                    }
                }

                btn->show(); // Important!
                m_cardButtons.push_back(btn);
            }
            currentX += CARD_W + SPACING;
        }
    }
}

void MainWindow::onCardClicked() {
    // Vom implementa asta in pasul urmator
}