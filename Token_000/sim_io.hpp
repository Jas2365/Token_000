_Pragma("once");

#include <iostream>
#include <queue>
#include <mutex>
#include <variant>
#include <string>
#include <thread>

#include "arith.hpp"
#include "number.hpp"
#include "Delivery_Net.hpp"

using Queue_Item = std::variant<number, arith, std::string>;

std::queue<std::string> message_queue;
std::mutex mtx;
std::condition_variable cv;

void input_loop() {
	std::string line;

	while (true) {
		std::getline(std::cin, line);

		{
			std::lock_guard<std::mutex> lock(mtx);
			message_queue.push(line);
		}
			cv.notify_one();
			if (line == "quit") break;
	}
}

void output_loop() {

	while (true) {
		Queue_Item current_item;
		{
			std::unique_lock<std::mutex> lock(mtx);
			cv.wait(lock, [] { return !message_queue.empty(); });

			current_item = message_queue.front();
			message_queue.pop();
		}
		// quit check
		if (std::holds_alternative<std::string>(current_item)) {
			if (std::get<std::string>(current_item) == "quit") return;
		}
		

		std::cout << "--------------------------\n";
	}
}

void sim_init() {
	std::cout << "Start: \n";

	std::thread inputthread(input_loop);
	std::thread outputthread(output_loop);

	inputthread.join();
	outputthread.join();
}
