#include <iostream>
#include <vector>
#include <variant>
#include <map>
#include <cmath>
#include <string>
#include <memory>
#include <fstream>
#include <algorithm>
#include <functional>

#include "number.hpp"
#include "arith.hpp"
#include "IModule.hpp"
#include "sim_io.hpp"
#include "Delivery_Net.hpp"
#include "parser.hpp"

// TODO: make a function to reuse add moduele to make a IMUL module in runtime
//	Diagram
//  
//	OP1 --|
//		  |--|
//  OP2 --|  |
//			 |--|
//				|--- Result
//  OP3 --------|


static bool options(int choice);

DeliveryNet net;

int main() {

	number nn("-1");
	nn.print();

	/*char in;
	while (1) {

		std::cout << "in: ";
		std::cin >> in;
		if (in == 'q') break;
		std::cout << "out: " << (char)std::tolower(in) << "\n";
	
	}*/
	//int choice = 0;


	//while (1) {
	//	std::cout << "\n=============\n";
	//	// << "1. List \n"
	//	// << "2. Add  \n"
	//	// << "0. Quit \n"
	//	// << "Select choice: ";
	//	//std::cin >> choice;

	//	//if (options(choice)) break;
	//	std::getline(std::cin, in);

	//	parser(in);
	//}

	//sim_init();

	//DeliveryNet net;

	//std::cout << "net current size " << (int)net.current_size << "\n";

	//net.add_unit(IModules::Add);
	//net.add_unit(IModules::Sub);

	//std::cout << "net current size " << (int)net.current_size << "\n";
	//number res;

	//res = net.deliver("*", number(0x23), number(0x21115));
	//res.print();

	return 0;
}
static bool options(int choice) {
	bool should_quit = false;
	switch (choice) {
	case 0:
		should_quit = true;
		break;
	case 1: 
		for (uint32_t i = 0; i < net.current_size; i++) {
			std::cout << net.functional_units[i].name << " | " << net.functional_units[i].symbol << "\n";
		}
		break;
	case 2: {
		std::string word;
		IModule m;
		std::cout << "enter name: ";
		std::cin >> word;
		m.name = word;

		std::cout << "enter symb: ";
		std::cin >> word;
		m.symbol = word;

		net.add_unit(m);
		break;
	}
	default:
		break;
	}
	return should_quit;
}