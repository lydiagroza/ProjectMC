#include "MainWindow.h"
#include <QApplication>
#include <QScreen>
#include <QVariant>
#include <QMouseEvent>
#include "CardNode.h"
#include <sstream>

Q_DECLARE_METATYPE(CardNode*);

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
    centralWidget->installEventFilter(this); // <-- Instalam filtrul de evenimente

    // --- STIL INITIAL (FUNDAL GENERAL) ---
    // #34495e = Un gri-albastru închis, arată ca o masă de joc modernă
    centralWidget->setStyleSheet("background-color: #34495e;");

    // --- TITLU ---
    titleLabel = new QLabel("7 WONDERS DUEL", centralWidget);
    titleLabel->setStyleSheet("color: white; font-size: 48px; font-weight: bold; background: transparent;");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setGeometry(0, 50, 1280, 80);

    // --- SUBTITLU ERA (Initial gol) ---
    ageLabel = new QLabel("", centralWidget);
    ageLabel->setStyleSheet("color: #f1c40f; font-size: 32px; font-weight: bold; background: transparent;");
    ageLabel->setAlignment(Qt::AlignCenter);
    ageLabel->setGeometry(0, 130, 1280, 50);

    // --- BUTON START ---
    startButton = new QPushButton("START GAME", centralWidget);
    startButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #27ae60; color: white;"
        "  font-size: 24px; font-weight: bold;"
        "  border-radius: 15px; border: 3px solid #219150;"
        "}"
        "QPushButton:hover { background-color: #2ecc71; }"
    );

    // Centrare Buton
    int btnW = 250, btnH = 80;
    startButton->setGeometry((1280 - btnW) / 2, (800 - btnH) / 2, btnW, btnH);

    connect(startButton, &QPushButton::clicked, this, &MainWindow::handleStartButton);

    // --- TOOLTIP LABEL ---
    m_tooltipLabel = new QLabel(centralWidget);
    m_tooltipLabel->setStyleSheet(
        "QLabel { "
        "  background-color: #f1c40f; "
        "  color: black; "
        "  font-weight: bold; "
        "  border: 2px solid black; "
        "  border-radius: 5px; "
        "  padding: 5px; "
        "}"
    );
    m_tooltipLabel->setAlignment(Qt::AlignCenter);
    m_tooltipLabel->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    m_tooltipLabel->hide();
}

MainWindow::~MainWindow() {
    clearPyramidUI();
}

// Funcție pentru a schimba textele ajutătoare în funcție de eră
void MainWindow::applyAgeStyle(int age) {
    if (age == 1) {
        ageLabel->setText("AGE I");
        ageLabel->setStyleSheet("color: #D7CCC8; font-size: 32px; font-weight: bold;"); // Bej deschis
    }
    else if (age == 2) {
        ageLabel->setText("AGE II");
        ageLabel->setStyleSheet("color: #AED6F1; font-size: 32px; font-weight: bold;"); // Albastru deschis
    }
    else if (age == 3) {
        ageLabel->setText("AGE III");
        ageLabel->setStyleSheet("color: #D2B4DE; font-size: 32px; font-weight: bold;"); // Mov deschis
    }
}

void MainWindow::handleStartButton() {
    // 1. Ascundem meniul de start
    startButton->hide();

    // Mutăm titlul mai sus
    titleLabel->setFont(QFont("Arial", 24, QFont::Bold));
    titleLabel->setGeometry(0, 10, 1280, 40);
    ageLabel->setGeometry(0, 50, 1280, 40);

    // 2. Inițializăm Backend-ul (Era 1)
    m_game.getSetup().startAge(1);

    // 3. Setăm stilul vizual
    applyAgeStyle(1);

    // 4. Desenăm Piramida
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

    // 1. Aflăm Era Curentă (Momentan 1)
    int currentAge = 1;

    // Culorile Erelor
    QString themeColor;
    if (currentAge == 1) themeColor = "#8D6E63";      // Maro (Age 1)
    else if (currentAge == 2) themeColor = "#2980B9"; // Albastru (Age 2)
    else themeColor = "#8E44AD";                      // Mov (Age 3)

    int centerX = this->width() / 2;
    int startY = 150;

    for (int r = 0; r < 20; ++r) {
        std::vector<CardNode*> rowNodes;
        for (int c = 0; c < 20; ++c) {
            CardNode* node = board.getNodeAt(r, c);
            if (!node) break;
            rowNodes.push_back(node);
        }
        if (rowNodes.empty()) break;

        // Calcule centrare
        int totalW = static_cast<int>(rowNodes.size()) * CARD_W + (static_cast<int>(rowNodes.size()) - 1) * SPACING;
        int currentX = centerX - (totalW / 2);
        int currentY = startY + (r * (CARD_H - OVERLAP));

        for (CardNode* node : rowNodes) {

            if (!node->isPlayed()) {
                QPushButton* btn = new QPushButton(centralWidget);
                btn->setProperty("cardNode", QVariant::fromValue(node));
                btn->setGeometry(currentX, currentY, CARD_W, CARD_H);

                // === STILIZARE NOUĂ PENTRU CONTURURI CLARE ===

                if (node->getFace() == Face::Down) {
                    // CAZ 1: SPATELE CĂRȚII (Face Down)
                    // Fundal: Culoarea Erei
                    // Contur: NEGRU (ca să nu se unească vizual)
                    btn->setText("");
                    btn->setStyleSheet(QString(
                        "QPushButton {"
                        "  background-color: %1;"  // Culoarea plină (ex: Maro)
                        "  border: 2px solid black;" // <--- CONTUR NEGRU CLAR
                        "  border-radius: 8px;"
                        "}"
                    ).arg(themeColor));
                }
                else {
                    // CAZ 2: FAȚA CĂRȚII (Face Up)
                    QString name = QString::fromStdString(node->getName());
                    if (name.length() > 9) name = name.left(7) + "..";
                    btn->setText(name);

                    if (node->isPlayable()) {
                        // DISPONIBILĂ
                        // Fundal Alb. 
                        // Folosim 'ridge' sau 'double' pentru a separa cartile suprapuse
                        // SAU 'border: 2px solid black' daca vrei contrast maxim.

                        // Varianta cu Ramă Colorată + Margine Neagră (simulată prin stil 'ridge')
                        btn->setStyleSheet(QString(
                            "QPushButton {"
                            "  background-color: white;"
                            "  color: black;"
                            "  font-weight: bold;"
                            "  border: 5px ridge %1;" // <--- RIDGE creează un efect 3D care separă cărțile
                            "  border-radius: 8px;"
                            "}"
                            "QPushButton:hover { background-color: #ecf0f1; }"
                        ).arg(themeColor));

                        connect(btn, &QPushButton::clicked, this, &MainWindow::onCardClicked);
                    }
                    else {
                        // BLOCATĂ
                        btn->setStyleSheet(QString(
                            "QPushButton {"
                            "  background-color: #ecf0f1;"
                            "  color: #7f8c8d;"
                            "  border: 2px dashed %1;" // Contur punctat colorat
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
    // Aici vom adăuga logica de cumpărare data viitoare
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    // Verificam daca evenimentul este pe un buton de carte
    QPushButton* btn = qobject_cast<QPushButton*>(watched);
    if (!btn || !m_cardButtons.empty() && std::find(m_cardButtons.begin(), m_cardButtons.end(), btn) == m_cardButtons.end()) {
        return QMainWindow::eventFilter(watched, event);
    }

    // Preluam nodul asociat (daca exista)
    QVariant property = btn->property("cardNode");
    if (!property.isValid()) {
        return QMainWindow::eventFilter(watched, event);
    }
    CardNode* node = property.value<CardNode*>();

    if (event->type() == QEvent::Enter)
    {
        // --- AFISAM TOOLTIP ---
        if (node && node->getCard() && node->getFace() == Face::Up)
        {
            std::shared_ptr<CardBase> card = node->getCard();
            std::map<Resource, std::uint8_t> cost = card->getCost();
            std::stringstream ss;

            ss << "Cost: ";
            if (cost.empty()) {
                ss << "Free";
            } else {
                for (auto const& [resource, amount] : cost) {
                    if (amount > 0) {
                        ss << to_string(resource) << "(" << (int)amount << ") ";
                    }
                }
            }
            
            m_tooltipLabel->setText(QString::fromStdString(ss.str()));
            
            // Pozitionam tooltip-ul deasupra butonului
            m_tooltipLabel->adjustSize(); // Calculam marimea mai intai
            QPoint pos = btn->mapToGlobal(QPoint((btn->width() - m_tooltipLabel->width())/2, -m_tooltipLabel->height() - 5));
            m_tooltipLabel->move(pos);
            m_tooltipLabel->show();
        }
    }
    else if (event->type() == QEvent::Leave)
    {
        // --- ASCUNDEM TOOLTIP ---
        m_tooltipLabel->hide();
    }

    return QMainWindow::eventFilter(watched, event);
}