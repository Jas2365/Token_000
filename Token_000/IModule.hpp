_Pragma("once");

#include <functional>
#include <string>
#include "number.hpp"

using math_fn = std::function<number(number, number)>;

struct IModule {
	std::string name;
	std::string symbol;
	int priority = 0;
	math_fn execute_logic;

	IModule() = default;
	IModule(std::string n, std::string s, math_fn logic, int p) : name(n), symbol(s), priority(p), execute_logic(logic) {}

	number execute(number a, number b) const {
		if (execute_logic) {
			return execute_logic(a, b);
		}
		return Numbers::zero;
	}

	~IModule() = default;
};

number add_fn(number a, number b) {
	return a + b;
}
number sub_fn(number a, number b) {
	return a - b;
}

namespace IModules {
	IModule Add("Addition",    "+", add_fn, 1);
	IModule Sub("Subtraction", "-", sub_fn, 1);
}