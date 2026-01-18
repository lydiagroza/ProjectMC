void MainWindow::updateTurnIndicator()
{
    if (!m_game) return;
    
    // Check who is the current player
    // If Player 1 is current, we might want the duck near Player 1 (Bottom)
    // If Player 2 (Opponent) is current, move duck near Opponent (Top)
    
    // However, in the current layout, the indicator is in the middle HBox.
    // To "pass" it, we can change its alignment or parent, OR simply change its color/style 
    // to indicate direction.
    
    // BETTER ANIMATION APPROACH:
    // Move the widget physically using QPropertyAnimation would require it to be a child of the central widget 
    // with absolute positioning, not inside the HBox layout.
    
    // SIMPLE LAYOUT APPROACH:
    // We can just change the border color to match the player color
    // Player 1 (Blue) vs Player 2 (Red)
    
    bool isPlayer1Turn = (m_game->getCurrentPlayer()->getName() == m_game->getPlayer1().getName());
    
    QString color = isPlayer1Turn ? "#1565C0" : "#8B0000"; // Blue vs Red
    
    ui->turnIndicator->setStyleSheet(
        QString("background-color: #FFD700; "
                "border: 4px solid %1; "
                "border-radius: 30px; "
                "font-size: 30px;")
        .arg(color)
    );
    
    // Optional: Tooltip text
    ui->turnIndicator->setToolTip(isPlayer1Turn ? "It is YOUR turn!" : "Opponent is thinking...");
}
