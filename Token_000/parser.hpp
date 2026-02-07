_Pragma("once");

#include <cstdint>
#include <cstring>
#include <string>
#include <iostream>
#include <variant>
#include <vector>
#include <istream>
#include <sstream>
#include <algorithm>

#include "arith.hpp"
#include "number.hpp"



void parser(std::string input) {

	std::vector<number> nums;
	std::stringstream ss(input);
	std::string word;
	while(ss >> word ) {
		//nums.push_back(from_string_number(word));
	}

	for (const auto& n : nums) {
		std::cout << " nn: " << n.get_str() << "\n";
	}

	return;
}