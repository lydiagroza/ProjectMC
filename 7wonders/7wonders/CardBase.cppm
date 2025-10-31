export module CardBase;
import <iostream>;
import <map>;
import <string>;
import <vector>;
import <optional>;
import <cstdint>;

namespace wonders7
{
    // -----------------------------
    // ENUMURI DE BAZĂ
    // -----------------------------

    export enum class Resource : std::uint8_t {
        Wood, Clay, Stone, Glass, Papyrus, Gold
    };

    export enum class Color : std::uint8_t {
        Brown, Gray, Yellow, Red, Blue, Green, Purple
    };

    export enum class Symbol : std::uint8_t {
        Barrel, Gear, Lamp, Scroll, Sword, Wall,
        Horse, Helmet, Mask, Sun, Moon, Balance
    };

    // -----------------------------
    // CLASA DE BAZĂ PENTRU CĂRȚI
    // -----------------------------

    export class CardBase {
    public:
        std::string m_name;
        std::uint16_t m_id: 16 ;
        Color m_color : 3;
        std::optional<Symbol> m_symbol;
        std::optional<std::vector<Symbol>> m_unlocks;
        std::map<Resource, std::uint8_t> m_cost;

        // ---- Getteri ----
        const std::string& get_name() const;
        std::uint16_t get_id() const;
        Color get_color() const;
        const std::map<Resource, std::uint8_t>& get_cost() const;
        const std::optional<Symbol>& get_symbol() const;
        const std::optional<std::vector<Symbol>>& get_unlocks() const;
    };

    // -----------------------------
    // OPERATOR <<
    // -----------------------------
    std::ostream& operator<<(std::ostream& os, const CardBase& card);

} // namespace wonders7
