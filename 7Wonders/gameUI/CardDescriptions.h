#pragma once
#include <QString>
#include <QMap>

class CardDescriptions
{
public:
    static QString getDescription(const QString& cardName)
    {
        static QMap<QString, QString> descriptions;

        if (descriptions.isEmpty()) {
            initializeDescriptions(descriptions);
        }

        QString normalized = cardName.toUpper().simplified();

        if (descriptions.contains(normalized)) {
            return descriptions[normalized];
        }

        return "This card provides strategic advantages for your civilization.";
    }

private:
    static void initializeDescriptions(QMap<QString, QString>& desc)
    {
        // ============ AGE I ============

        // Brown Cards (Resources)
        desc["LUMBER YARD"] = "Produces 1 Wood.";
        desc["QUARRY"] = "Produces 1 Stone.";
        desc["STONE PIT"] = "Produces 1 Stone.";
        desc["LOGGING CAMP"] = "Produces 1 Wood.";
        desc["CLAY POOL"] = "Produces 1 Clay.";
        desc["CLAY PIT"] = "Produces 1 Clay.";

        // Grey Cards (Manufactured Goods)
        desc["PRESS"] = "Produces 1 Papyrus.";
        desc["GLASSWORKS"] = "Produces 1 Glass.";

        // Green Cards (Science)
        desc["APOTHECARY"] = "Worth 1 victory point and grants one scientific symbol (Wheel).";
        desc["WORKSHOP"] = "Worth 1 victory point and grants one scientific symbol (Scales).";
        desc["SCRIPTORIUM"] = "Grants one scientific symbol (Ink).";
        desc["PHARMACIST"] = "Grants one scientific symbol (Mortar).";

        // Blue Cards (Civilian)
        desc["THEATER"] = "Worth 3 victory points.";
        desc["BATHS"] = "Worth 3 victory points.";
        desc["ALTAR"] = "Worth 3 victory points.";

        // Yellow Cards (Commercial)
        desc["TAVERN"] = "Grants 4 coins.";
        desc["STONE RESERVE"] = "Changes trading rules; stone purchase price is set to 1 coin.";
        desc["CLAY RESERVE"] = "Changes trading rules; clay purchase price is set to 1 coin.";
        desc["WOOD RESERVE"] = "Changes trading rules; wood purchase price is set to 1 coin.";

        // Red Cards (Military)
        desc["STABLE"] = "Grants 1 military shield.";
        desc["GUARD TOWER"] = "Grants 1 military shield.";
        desc["GARRISON"] = "Grants 1 military shield.";
        desc["PALISADE"] = "Grants 1 military shield.";

        // ============ AGE II ============

        // Brown Cards
        desc["BRICKYARD"] = "Produces 2 Clays.";
        desc["SAWMILL"] = "Produces 2 Woods.";
        desc["SHELF QUARRY"] = "Produces 2 Stones.";

        // Grey Cards
        desc["GLASS-BLOWER"] = "Produces 1 Glass.";
        desc["DRYING ROOM"] = "Produces 1 Papyrus.";

        // Blue Cards
        desc["ROSTRUM"] = "Worth 4 victory points.";
        desc["TEMPLE"] = "Worth 4 victory points.";
        desc["AQUEDUCT"] = "Worth 5 victory points.";
        desc["COURTHOUSE"] = "Worth 5 victory points.";
        desc["STATUE"] = "Worth 4 victory points.";

        // Green Cards
        desc["LIBRARY"] = "Worth 2 victory points and grants one scientific symbol (Ink).";
        desc["DISPENSARY"] = "Worth 2 victory points and grans one scientific symbol(mortar) .";
        desc["LABORATORY"] = "Worth 1 victory point and grants one scientific symbol (Scales).";
        desc["SCHOOL"] = "Worth 1 victory point and grants one scientific symbol (Wheel).";

        // Red Cards
        desc["WALLS"] = "Grants 2 military shields.";
        desc["ARCHERY RANGE"] = "Grants 2 military shields.";
        desc["PARADE GROUND"] = "Grants 2 military shields.";
        desc["BARRACKS"] = "Grants 1 military shield.";
        desc["HORSE BREEDERS"] = "Grants 1 military shield.";

        // Yellow Cards
        desc["BREWERY"] = "Grants 6 coins.";
        desc["CUSTOMS HOUSE"] = "Changes trading rules for manufactured goods (Glass/Papyrus); purchase price is set to 1 coin.";
        desc["FORUM"] = "Produces either 1 Glass or 1 Papyrus each turn.";
        desc["CARAVANSERY"] = "Produces either 1 Wood, 1 Clay, or 1 Stone each turn.";

        // ============ AGE III ============

        // Blue Cards
        desc["PANTHEON"] = "Worth 6 victory points.";
        desc["PALACE"] = "Worth 7 victory points.";
        desc["GARDENS"] = "Worth 6 victory points.";
        desc["SENATE"] = "Worth 5 victory points.";
        desc["OBELISK"] = "Worth 5 victory points.";
        desc["TOWN HALL"] = "Worth 7 victory points.";

        // Green Cards
        desc["UNIVERSITY"] = "Worth 2 victory points and grants one scientific symbol (Gyroscope).";
        desc["OBSERVATORY"] = "Worth 2 victory points and grants one scientific symbol (Gyroscope).";
        desc["ACADEMY"] = "Worth 3 victory points and grants one scientific symbol (Sun Dial).";
        desc["STUDY"] = "Worth 3 victory points and grants one scientific symbol (Sun Dial).";

        // Red Cards
        desc["PRETORIUM"] = "Grants 3 military shields.";
        desc["ARSENAL"] = "Grants 3 military shields.";
        desc["CIRCUS"] = "Grants 2 military shields.";
        desc["FORTIFICATIONS"] = "Grants 2 military shields.";
        desc["SIEGE WORKSHOP"] = "Grants 2 military shields.";

        // Yellow Cards with Instant Coins
        desc["ARMORY"] = "Worth 3 victory points and grants 1 coin for each military (red) card in your city upon construction.";
        desc["CHAMBER OF COMMERCE"] = "Worth 3 victory points and grants 3 coins for each manufactured goods (grey) card in your city upon construction.";
        desc["LIGHTHOUSE"] = "Worth 3 victory points and grants 1 coin for each commercial (yellow) card in your city upon construction.";
        desc["ARENA"] = "Worth 3 victory points and grants 2 coins for each Wonder constructed in your city upon construction.";
        desc["PORT"] = "Worth 3 victory points and grants 2 coins for each raw material (brown) card in your city upon construction.";

        // ============ GUILDS ============

        desc["BUILDERS_GUILD"] = "Worth 2 victory points for each Wonder constructed in the city with the most wonders.";
        desc["MONEYLENDERS_GUILD"] = "Worth 1 victory point for each set of 3 coins in the richest city.";
        desc["SCIENTISTS_GUILD"] = "Worth 1 victory point and grants 1 coin for each green card in the city with the most green cards.";
        desc["TRADERS_GUILD"] = "Worth 1 victory point and grants 1 coin for each yellow card in the city with the most yellow cards.";
        desc["MAGISTRATES_GUILD"] = "Worth 1 victory point and grants 1 coin for each blue card in the city with the most blue cards.";
        desc["TACTICIANS_GUILD"] = "Worth 1 victory point and grants 1 coin for each red card in the city with the most red cards.";
        desc["SHIPOWNERS_GUILD"] = "Worth 1 victory point and grants 1 coin for each brown and grey card in the city with the most brown and grey cards.";
    }
};
