#pragma once
#include <vector>
#include "Resource.h"


// pentru o singura resursa
struct CostItem { 
	Resource resource;
	int amount;
};

//pentru o list de resurse
using Cost = std::vector<CostItem>;
