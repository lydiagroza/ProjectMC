#include "WonderLoader.h";

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include "CardLoader.h"

static std::vector<std::shared_ptr<Wonder>> loadFromCSV(const std::string& filename) {
	using namespace std;
	vector<shared_ptr<Wonder>> wonders;
	ifstream file(filename);
	if (!file.is_open()) {
		cerr << "Eroare, nu s-a putut deschide fisierul " << filename << endl;
		return wonders;
	}

	string line;
	getline(file, line);

	while (getline(file, line)) {
		stringstream ss(line);
		string name, idStr, costStr, effectsStr;

		getline(ss, name, ',');
		getline(ss, idStr, ',');
		getline(ss, costStr, ',');
		getline(ss, effectsStr, '\n');

		auto trim = [](string s) {
			s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
			s.erase(remove(s.begin(), s.end(), '"'), s.end());
			return s;
			};

		name = trim(name);
		idStr = trim(idStr);
		costStr = trim(costStr);
		effectsStr = trim(effectsStr);

		uint16_t id = static_cast<uint16_t>(stoi(idStr));
		map<Resource, uint8_t> cost = Wonder::parseCost(costStr);

		auto wonder = make_shared<Wonder>(name, id, cost);

		auto effects = Wonder::ParseEffects(effectsStr);
		for (auto& ef : effects)
			wonder->addEffect(ef);
		wonders.push_back(wonder)
	}
}