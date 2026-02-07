_Pragma("once");

#include <string>
#include <map>
#include "IModule.hpp"

struct DeliveryNet {
	std::vector<IModule> functional_units;
	std::map<std::string, uint32_t> routing_table;

	uint32_t current_size = 0;

	DeliveryNet() = default;

	void add_unit(IModule m) {
		functional_units.push_back(m);
		routing_table[m.symbol] = current_size;
		current_size++;
	}
	// for adding units in runtime
	void add_unit(std::string name, std::string symbol) {}

	number deliver(std::string symbol, number a, number b) {
		if (routing_table.count(symbol)) {
			uint32_t dest_id = routing_table[symbol];
			return functional_units[dest_id].execute(a, b);
		}
		std::cerr << "[UNKNOWN SIGNAL]:: No route for signal [ " << symbol << " ]\n";
		return Numbers::zero;
	}
};
