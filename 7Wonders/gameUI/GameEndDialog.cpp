#include "GameEndDialog.h"
#include "ui_GameEndDialog.h"
#include "Player.h"
#include "GuildEffects.h"
#include <map>

GameEndDialog::GameEndDialog(Game* game, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameEndDialog)
{
    ui->setupUi(this);
    setupStats(game);
    connect(ui->btnClose, &QPushButton::clicked, this, &QDialog::accept);
}

GameEndDialog::~GameEndDialog()
{
    delete ui;
}

void GameEndDialog::setupStats(Game* game)
{
    if (!game) return;

    Player& p1 = game->getPlayer1();
    Player& p2 = game->getPlayer2();

    ui->p1NameLabel->setText(QString::fromStdString(p1.getName()));
    ui->p2NameLabel->setText(QString::fromStdString(p2.getName()));

    int vp1 = game->calculatePlayerVP(p1);
    int vp2 = game->calculatePlayerVP(p2);

    ui->p1StatsLabel->setText(formatStats(p1, p2, vp1));
    ui->p2StatsLabel->setText(formatStats(p2, p1, vp2));

    std::optional<Player> winner = game->determinateWinner();
    if (winner) {
        ui->winnerLabel->setText("🏆 THE WINNER IS: " + QString::fromStdString(winner->getName()) + " 🏆");
    } else {
        ui->winnerLabel->setText("🤝 IT'S A DRAW! 🤝");
    }
}

QString GameEndDialog::formatStats(const Player& p, const Player& opp, int totalVP)
{
    auto getCount = [&](Color c) {
        auto inv = p.getInventory();
        return inv.count(c) ? (int)inv.at(c).size() : 0;
    };

    QString stats;
    stats += QString("Total VP: %1\n\n").arg(totalVP);
    stats += QString("💰 Coins: %1\n").arg(p.getCoins());
    stats += QString("🟦 Blue Cards: %1\n").arg(getCount(Color::Blue));
    stats += QString("🟥 Red Cards: %1\n").arg(getCount(Color::Red));
    stats += QString("🟩 Green Cards: %1\n").arg(getCount(Color::Green));
    stats += QString("🟨 Yellow Cards: %1\n").arg(getCount(Color::Yellow));
    stats += QString("🟫 Brown Cards: %1\n").arg(getCount(Color::Brown));
    stats += QString("⬜ Gray Cards: %1\n").arg(getCount(Color::Gray));
    
    // Guild Breakdown
    const auto& inv = p.getInventory();
    if (inv.count(Color::Purple)) {
        int guildCount = inv.at(Color::Purple).size();
        stats += QString("\n🟪 Guilds (%1):\n").arg(guildCount);
        
        for (const auto& card : inv.at(Color::Purple)) {
            int pts = 0;
            int id = card->getId();
            switch (id) {
                case 67: pts = GuildEndGameEffects::calculateBuildersGuild(p, opp); break;
                case 68: pts = GuildEndGameEffects::calculateMoneylendersGuild(p, opp); break;
                case 69: pts = GuildEndGameEffects::calculateScientistsGuild(p, opp); break;
                case 70: pts = GuildEndGameEffects::calculateShipownersGuild(p, opp); break;
                case 71: pts = GuildEndGameEffects::calculateTradersGuild(p, opp); break;
                case 72: pts = GuildEndGameEffects::calculateMagistratesGuild(p, opp); break;
                case 73: pts = GuildEndGameEffects::calculateTacticiansGuild(p, opp); break;
                default: pts = 0; break;
            }
            stats += QString("   • %1: %2 VP\n").arg(QString::fromStdString(card->getName())).arg(pts);
        }
    } else {
        stats += QString("\n🟪 No Guilds\n");
    }

    return stats;
}
