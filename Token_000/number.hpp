_Pragma("once");

#include <vector>
#include "digit.hpp"

inline static void to_digit(int src, int base, std::vector<Digit>& dest)
{
	do {
		dest.push_back(Digits::all_digits[src % base]);
		src /= base;
	} while (src != 0);
}

inline static void to_digit(std::string src, bool neg, int& base, std::vector<Digit>& dest) {
	size_t start = neg ? 1 : 0;

	if (src.size() >= start + 2 && src[start] == '0' && std::tolower(src[start + 1]) == 'x') {
		start += 2;
		base = 16;
	} else {
		base = 10;
	}
	
	for ( int ridx = (int)src.size() -1; ridx >= (int)start; ridx--) {
		char c = (char)std::tolower(src[ridx]);
		
		for (const auto& d : Digits::all_digits) {
			if (d.char_code == c && d.value < (uint32_t)(base)) {
				dest.push_back(d);
				break;
			}
 					
		}
	}
}

struct number {
	std::vector<Digit> nums;
	int base = 16;
	bool is_negative = false;

	number() = default;
	number(std::string n, int base = 16) : base(base) {
		if (n[0] == '-') {
			is_negative = true;
		}
		else {
			is_negative = false;
		}

		to_digit(n, is_negative, this->base, nums);

	}
	number(int32_t n, int base = 16) : base(base) {
		if (n < 0) {
			n = -n;
			is_negative = true;
		}
		else {
			is_negative = false;
		}
		
		to_digit(n, base, nums);

	}

	number operator+(const number& other) const {
		return number(get_val() + other.get_val());
	}
	number operator-(const number& other) const {
		return number(get_val() - other.get_val());
	}

	std::string get_str() const {
		std::string str = "";
		if (is_negative) str += "-";
		if(base == 16) {
			str += "0x";
		}
		if (nums.empty()) { str += "0"; }
		for (auto it = nums.rbegin(); it != nums.rend(); it++) {
			str += char((*it).char_code);
		}
		return str;
	}
	int32_t get_val() const {
		int32_t val = 0;
		if (nums.empty()) { return val; }
		
		for (auto it = nums.rbegin(); it != nums.rend(); it++) {
			val = val * base;
			val += (*it).value;
		}
		return is_negative ? -val : val;
	}

	void print() {
		std::cout << "str : " << get_str() << "\n";
		std::cout << "val : ";
		if (base == 16) std::cout << "0x" << std::hex << get_val() << "\n";
		else std::cout << std::dec << get_val() << "\n";
	}
};

namespace Numbers {
	number zero(0);
}

static number for_decimal(const std::string& input) {

	std::string numeric_part = "";
	bool negative = false;
	bool found_digit = false;

	for (size_t i = 0; i < input.length(); i++) {
		char c = input[i];

		if (c == '-' && !found_digit) {
			negative = true;
			continue;
		}

		if (isdigit(static_cast<unsigned char>(c))) {
			numeric_part += c;
			found_digit = true;
		}
		else if (found_digit) {
			break;
		}
	}

	if (numeric_part.empty()) return Numbers::zero;

	int32_t val = std::stoi(numeric_part);

	return number(val);
}

number for_hexadecimal(const std::string& input) {
	std::string hex_part = "";
	bool negative = false;
	bool found_digit = false;

	for (size_t i = 0; i < input.length(); i++) {
		char c = input[i];

		if (c == '-' && !found_digit) {
			negative = true;
			continue;
		}

		if ((c == '0') && ((i + 1 < input.length()) && std::tolower(input[i + 1]) == 'x')) {
			i++;
			continue;
		}
		if (isxdigit(static_cast<unsigned char>(c))) {
			hex_part += c;
			found_digit = true;
		}
		else if (found_digit) {
			break;
		}
	}

	if (hex_part.empty()) return Numbers::zero;

	int32_t val = static_cast<int32_t>(std::stoul(hex_part, nullptr, 16));

	return number(val);
}

static number from_string_number(const std::string& input) {
	
	


	return number(0);
}