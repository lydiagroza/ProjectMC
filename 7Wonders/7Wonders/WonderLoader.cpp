#include "WonderLoader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

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

	}
}