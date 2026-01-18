#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Game.h"
#include "Player.h"

void MainWindow::updateTurnIndicator()
{
    if (!m_game) return;
    
    bool isPlayer1Turn = (m_game->getCurrentPlayer()->getName() == m_game->getPlayer1().getName());
    
    QString color = isPlayer1Turn ? "#1565C0" : "#8B0000"; 
    
    ui->turnIndicator->setStyleSheet(
        QString("background-color: #FFD700; "
                "border: 4px solid %1; "
                "border-radius: 30px; "
                "font-size: 30px;")
        .arg(color)
    );
    
    ui->turnIndicator->setToolTip(isPlayer1Turn ? "It is YOUR turn!" : "Opponent is thinking...");
}
