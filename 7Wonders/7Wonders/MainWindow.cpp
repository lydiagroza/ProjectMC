#include "MainWindow.h"
#include <QString> // Needed to convert std::string to Qt text

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    // 1. Setup Window
    this->setWindowTitle("7 Wonders - Qt Interface");
    this->resize(800, 600);

    // 2. Setup UI Elements
    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    layout = new QVBoxLayout(centralWidget);

    infoLabel = new QLabel("Initializing...", centralWidget);
    // Make the font bigger so you can see it easily
    QFont font = infoLabel->font();
    font.setPointSize(16);
    infoLabel->setFont(font);
    infoLabel->setText("Current Player: Player 1");
    infoLabel->setAlignment(Qt::AlignCenter); // Center the text

    infoLabel->setStyleSheet(
        "QLabel {"
        "  color: #2c3e50;"          // Dark Blue text
        "  font-size: 24px;"         // Bigger font
        "  font-weight: bold;"       // Bold
        "  background-color: #ecf0f1;" // Light gray background
        "  padding: 10px;"           // Space around text
        "  border-radius: 5px;"      // Rounded corners
        "}"
    );

    startButton = new QPushButton("Start Game", centralWidget);

    layout->addWidget(infoLabel);
    layout->addWidget(startButton);

    connect(startButton, &QPushButton::clicked, this, &MainWindow::handleStartButton);

    // 3. Initialize the Game Engine
    m_game.initialize();


    // 4. TEST: Pull the name from the Game and show it on screen
    std::string pName = m_game.getCurrentPlayer()->getName();
    infoLabel->setText("Current Player: " + QString::fromStdString(pName));
}

MainWindow::~MainWindow() {
    // Qt cleans up children automatically
}

void MainWindow::handleStartButton() {
    // We can also update it when you click the button
    infoLabel->setText("Button Clicked! Game is running.");
}