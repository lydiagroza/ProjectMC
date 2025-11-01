#include"Player.h"

Player::Player(const std::string& playerName) :name(playerName), nrCoins(7), militaryScore(0), rawMaterials{}, manufacturedGoods{}, fixedCost{}, bonusProduction{}, cityBuildings{}, availableWonders{}, builtWonders{}, scientificSymbols{}, chainSymbols{} {
}
void Player::addCoins(int amount) {
	nrCoins += amount;
}
bool Player::decreaseCoins(int amount) {
	if (nrCoins >= amount) {
		nrCoins -= amount;
		return true;
	}
	return false;
}
//void Player::moveMilitaryScore(int shields,Player&opponent) {
//	militaryScore += shields;
//	if (militaryScore > 9) {
//		militaryScore = 9;
//	}
//	if (militaryScore == 9) {
//		return;
//	}
//}

//Player::addCards(const Card& card, Players& opponent, GameState& board) {
//	cityBuildings[card.color].pushback(card);
//}


