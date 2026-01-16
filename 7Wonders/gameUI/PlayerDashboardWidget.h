#pragma once

#include <QWidget>
#include <vector>
#include <map>
#include <cstdint>
#include "CardBase.h"
#include "Wonder.h"

namespace Ui {
class PlayerDashboardWidget;
}

class PlayerDashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerDashboardWidget(QWidget* parent = nullptr);
    ~PlayerDashboardWidget();

    // Sets the visual theme (Red for Opponent, Blue for Player)
    void setTheme(bool isOpponent);

    // Updates the displayed data
    void updateDashboard(const std::string& name, int coins, 
                         const std::map<Resource, std::uint8_t>& resources,
                         const std::vector<std::shared_ptr<Wonder>>& wonders);

private:
    Ui::PlayerDashboardWidget *ui;
};
