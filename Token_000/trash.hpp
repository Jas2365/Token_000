_Pragma("once");

#include <variant>

#include "number.hpp"
#include "arith.hpp"

#include <queue>
#include <mutex>
#include <string>

// logic calculator

/*
*   memory holds variant of numbers and arith operators
*
* algorithm
*	if one number is given Then
*		the number is stored in the memory and output immediately
*	if an operator is given Then      || alternate if a number is given it is discarded
*		the operator is stored in the memory
*	if a number is given Then		  || alternate if an operator is given then it is discarded
*		the number is operated on by the operator and number stored immediately before and immediately outputs the result
*/

/*
* a nn has neuron val and weights connections
* no neuron val only weights/gates similar to an alu?
*
*/

using Queue_Item = std::variant<number, arith, std::string>;

std::queue<std::string> message_queue;
std::mutex mtx;
std::condition_variable cv;

void input_loop() {
	std::string line;

	while (true) {
		std::getline(std::cin, line);

		Queue_Item item;
		bool valid = false;

		if (line == "quit") {
			item = line;
			valid = true;
		}

		// op check
		else if (line == "+") { item = Arith::plus; valid = true; }
		else if (line == "-") { item = Arith::minus; valid = true; }
		else if (line == "*") { item = Arith::multiply; valid = true; }
		else if (line == "/") { item = Arith::divide; valid = true; }

		// number check
		else if (!line.empty() && (isdigit(line[0]) || line[0] == '-')) {
			try {
				int val = std::stoi(line);
				item = number(val);
				valid = true;
			}
			catch (...) { valid = false; }
		}

		if (valid) {
			{
				std::lock_guard<std::mutex> lock(mtx);
				message_queue.push(item);
			}
			cv.notify_one();
			if (line == "quit") break;
		}
		else {
			std::cout << "Invalid Input. Try '10', '+' or 'quit'\n";
		}
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
struct Memory {
	// slot 0: left operand / result;
	// slot 1: operator
	// slot 2: right operand / processing slot
	std::variant<number, arith> data[3]{};
	bool has_result = false;
	bool has_op = false;
};

class nn {
private:
	Memory mem;

	void handle_number(number n) {
		// Logic:
		// 1. If we have no operator in waititng -> store this number a new base (slot 0)
		// 2. If we do have an operator in waiting -> Perform math (slot 0 , op, n)

		if (!mem.has_op) {
			// Start:
			mem.data[0] = n;
			mem.has_result = true;
			std::cout << ">> Stored: "; n.print(); std::cout << "\n";
		}
		else {
			// execute 
			number left = std::get<number>(mem.data[0]);
			arith op = std::get<arith>(mem.data[1]);

			int l_val = left.get_val();
			int r_val = n.get_val();
			int res_val = 0;

			switch (op.op) {
			case add_op: res_val = l_val + r_val; break;
			case sub_op: res_val = l_val - r_val; break;
			case mul_op: res_val = l_val * r_val; break;
			case div_op: res_val = (r_val != 0) ? l_val / r_val : 0; break;
			}

			number result(res_val);

			// Output:
			std::cout << ">> ";
			left.print(); std::cout << " " << (char)op.char_code << " ";
			n.print(); std::cout << " = ";
			result.print(); std::cout << "\n";

			// clean up
			mem.data[0] = result;
			mem.has_op = false;
		}
	}
	void handle_arith(arith a) {
		// Logic:
		// If we have a number in Slot 0, we can accept an operator.
		// If we don't have a number yet, we usually discard the operator (or treat as 0 op X)
		if (mem.has_result) {
			mem.data[1] = a;
			mem.has_op = true;
			std::cout << ">> Operator Set: " << (char)a.char_code << "\n";
		}
		else {
			std::cout << ">> Ignored: Need a number first.\n";
		}
	}

public:
	nn() = default;

	void process(const Queue_Item& item) {
		if (std::holds_alternative<number>(item)) {
			handle_number(std::get<number>(item));
		}
		else if (std::holds_alternative<arith>(item)) {
			handle_arith(std::get<arith>(item));
		}
	}

};



// =========================================================================
#include <map>

// --- The Learning Neuron (Module) ---
struct IModule {
	virtual number execute(number a, number b) = 0;
	virtual void learn(int32_t a, int32_t b, int32_t res) = 0;
	virtual ~IModule() = default;
};

class DeepLearningModule : public IModule {
	// A tiny 2-layer network: 2 inputs -> 4 hidden neurons -> 1 output
	float weights1[2][4]; // Weights from input to hidden
	float weights2[4];    // Weights from hidden to output
	float learning_rate = 0.001f;

public:
	DeepLearningModule() {
		// Initialize with random small values
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 4; j++) weights1[i][j] = ((rand() % 100) / 100.0f);
		for (int i = 0; i < 4; i++) weights2[i] = ((rand() % 100) / 100.0f);
	}

	number execute(number a, number b) override {
		float inA = (float)a.get_val();
		float inB = (float)b.get_val();

		// Hidden Layer with ReLU activation
		float hidden[4] = { 0 };
		for (int j = 0; j < 4; j++) {
			hidden[j] = (inA * weights1[0][j]) + (inB * weights1[1][j]);
			if (hidden[j] < 0) hidden[j] = 0; // ReLU activation
		}

		// Output Layer
		float out = 0;
		for (int j = 0; j < 4; j++) out += hidden[j] * weights2[j];

		return number((int32_t)std::round(out));
	}

	void learn(int32_t a, int32_t b, int32_t target) override {
		// Training needs more iterations for non-linear math
		for (int iter = 0; iter < 5000; iter++) {
			float inA = (float)a;
			float inB = (float)b;

			// Forward pass
			float hidden[4];
			for (int j = 0; j < 4; j++) {
				hidden[j] = (inA * weights1[0][j]) + (inB * weights1[1][j]);
				if (hidden[j] < 0) hidden[j] = 0;
			}
			float prediction = 0;
			for (int j = 0; j < 4; j++) prediction += hidden[j] * weights2[j];

			float error = (float)target - prediction;

			// Backpropagation (Updating weights)
			for (int j = 0; j < 4; j++) {
				// Update hidden-to-output
				weights2[j] += learning_rate * error * hidden[j];

				// Update input-to-hidden (only if neuron was active)
				if (hidden[j] > 0) {
					weights1[0][j] += learning_rate * error * weights2[j] * inA;
					weights1[1][j] += learning_rate * error * weights2[j] * inB;
				}
			}
		}
	}
};
//class NeuronModule : public IModule {
//    float w1 = 0.0f; // Weight for input A
//    float w2 = 0.0f; // Weight for input B
//    float learning_rate = 0.01f;
//
//public:
//    // This is the "Inference" phase
//    number execute(number a, number b) override {
//        float out = (w1 * a.get_val()) + (w2 * b.get_val());
//        return number((int32_t)std::round(out));
//    }
//
//    // This is the "Training" phase (Stochastic Gradient Descent)
//    void learn(int32_t a, int32_t b, int32_t target) override {
//        for (int i = 0; i < 500; i++) { // Iterative refinement
//            float prediction = (w1 * a) + (w2 * b);
//            float error = target - prediction;
//
//            // Adjust weights based on error
//            w1 += learning_rate * error * a;
//            w2 += learning_rate * error * b;
//        }
//    }
//};

// --- The Registry (The Array) ---
struct ALU {
	std::shared_ptr<IModule> slots[256]; // 256 "Opcode" slots
	std::map<std::string, uint8_t> symbol_to_id;

	void learn_new_op() {
		std::string sym;
		int id;
		std::cout << "\n[LEARNING MODE]\nEnter symbol (e.g., -): ";
		std::cin >> sym;
		std::cout << "Enter an ID (0-255): ";
		std::cin >> id;

		//auto neuron = std::make_shared<NeuronModule>();
		auto neuron = std::make_shared<DeepLearningModule>();

		std::cout << "Provide 2 examples to train the neuron:\n";
		for (int i = 0; i < 2; i++) {
			int32_t a, b, res;
			std::cout << "Example " << i + 1 << " (InputA InputB Result): ";
			std::cin >> a >> b >> res;
			neuron->learn(a, b, res);
		}

		slots[id] = neuron;
		symbol_to_id[sym] = (uint8_t)id;
		std::cout << "Successfully learned '" << sym << "' at slot " << id << "!\n";
	}

	number run(std::string sym, number a, number b) {
		if (symbol_to_id.count(sym)) {
			return slots[symbol_to_id[sym]]->execute(a, b);
		}
		std::cout << "Unknown symbol!\n";
		return number(0);
	}
};

// --- Execution ---
int main() {
	ALU my_alu;
	bool running = true;

	while (running) {
		std::cout << "\n1. Learn New Format\n2. Use ALU\n3. Exit\nChoice: ";
		int choice; std::cin >> choice;

		if (choice == 1) {
			my_alu.learn_new_op();
		}
		else if (choice == 2) {
			std::string op; int32_t v1, v2;
			std::cout << "Enter expression (Value1 Op Value2): ";
			std::cin >> v1 >> op >> v2;
			number result = my_alu.run(op, number(v1), number(v2));
			std::cout << "ALU Output: " << result.get_val() << std::endl;
		}
		else {
			running = false;
		}
	}
	return 0;
}

// ======================== Trash V2 =====================================
// --- Module Interface ---
struct IModule {
	virtual number execute(number a, number b) = 0;
	virtual void learn(int32_t a, int32_t b, int32_t target) = 0;
	virtual void save(std::ofstream& out) = 0;
	virtual void load(std::ifstream& in) = 0;
	virtual ~IModule() = default;
};

// --- The Deep Learning "Neuron" Module ---
class DeepLearningModule : public IModule {
	static const int HIDDEN_SIZE = 16;
	float weights1[2][HIDDEN_SIZE];
	float weights2[HIDDEN_SIZE];
	float bias1[HIDDEN_SIZE];
	float bias2 = 0.0f;
	float learning_rate = 0.0000000001f;

public:
	DeepLearningModule() {
		for (int j = 0; j < HIDDEN_SIZE; j++) {
			weights1[0][j] = ((rand() % 2000) / 1000.0f) - 1.0f;
			weights1[1][j] = ((rand() % 2000) / 1000.0f) - 1.0f;
			weights2[j] = ((rand() % 2000) / 1000.0f) - 1.0f;
			bias1[j] = 0.0f;
		}
	}

	number execute(number a, number b) override {
		float inA = (float)a.get_val();
		float inB = (float)b.get_val();
		float hidden[HIDDEN_SIZE];

		for (int j = 0; j < HIDDEN_SIZE; j++) {
			float sum = (inA * weights1[0][j]) + (inB * weights1[1][j]) + bias1[j];
			hidden[j] = std::max(0.0f, sum); // ReLU activation
		}

		float out = bias2;
		for (int j = 0; j < HIDDEN_SIZE; j++) out += hidden[j] * weights2[j];
		return number((int32_t)std::round(out));
	}

	void learn(int32_t a, int32_t b, int32_t target) override {
		for (int iter = 0; iter < 100; iter++) {
			float inA = (float)a;
			float inB = (float)b;

			// Forward
			float hidden[HIDDEN_SIZE];
			for (int j = 0; j < HIDDEN_SIZE; j++) {
				hidden[j] = std::max(0.0f, (inA * weights1[0][j]) + (inB * weights1[1][j]) + bias1[j]);
			}
			float pred = bias2;
			for (int j = 0; j < HIDDEN_SIZE; j++) pred += hidden[j] * weights2[j];

			float error = (float)target - pred;
			if (error > 50.0f) error = 50.0f; // Gradient Clipping
			if (error < -50.0f) error = -50.0f;

			// Backprop
			bias2 += learning_rate * error;
			for (int j = 0; j < HIDDEN_SIZE; j++) {
				weights2[j] += learning_rate * error * hidden[j];
				if (hidden[j] > 0) {
					bias1[j] += learning_rate * error * weights2[j];
					weights1[0][j] += learning_rate * error * weights2[j] * inA;
					weights1[1][j] += learning_rate * error * weights2[j] * inB;
				}
			}
		}
	}

	void save(std::ofstream& out) override {
		for (int i = 0; i < 2; i++) for (int j = 0; j < HIDDEN_SIZE; j++) out << weights1[i][j] << " ";
		for (int j = 0; j < HIDDEN_SIZE; j++) out << weights2[j] << " ";
		for (int j = 0; j < HIDDEN_SIZE; j++) out << bias1[j] << " ";
		out << bias2 << "\n";
	}

	void load(std::ifstream& in) override {
		for (int i = 0; i < 2; i++) for (int j = 0; j < HIDDEN_SIZE; j++) in >> weights1[i][j];
		for (int j = 0; j < HIDDEN_SIZE; j++) in >> weights2[j];
		for (int j = 0; j < HIDDEN_SIZE; j++) in >> bias1[j];
		in >> bias2;
	}
};

// --- The Learning ALU ---
struct ALU {
	std::shared_ptr<IModule> slots[256];
	std::map<std::string, uint8_t> symbol_to_id;

	void learn_from_file() {
		std::string path, sym; int id;
		std::cout << "File path: "; std::cin >> path;
		std::cout << "Symbol: "; std::cin >> sym;
		std::cout << "Slot (0-255): "; std::cin >> id;

		std::ifstream file(path);
		if (!file) { std::cout << "File not found!\n"; return; }

		std::vector<std::vector<int32_t>> data;
		int32_t a, b, r;
		while (file >> a >> b >> r) data.push_back({ a, b, r });

		auto neuron = std::make_shared<DeepLearningModule>();
		std::cout << "Training...";
		for (int epoch = 0; epoch < 2000; epoch++) {
			for (auto& row : data) neuron->learn(row[0], row[1], row[2]);
		}

		slots[id] = neuron;
		symbol_to_id[sym] = (uint8_t)id;
		std::cout << " Done.\n";
	}

	void save_all(std::string path) {
		std::ofstream out(path);
		out << symbol_to_id.size() << "\n";
		for (auto const& [s, i] : symbol_to_id) out << s << " " << (int)i << "\n";
		for (int i = 0; i < 256; i++) if (slots[i]) { out << i << " "; slots[i]->save(out); }
	}

	void load_all(std::string path) {
		std::ifstream in(path);
		if (!in) return;
		size_t sz; in >> sz;
		for (size_t i = 0; i < sz; i++) { std::string s; int id; in >> s >> id; symbol_to_id[s] = id; }
		int id; while (in >> id) { auto n = std::make_shared<DeepLearningModule>(); n->load(in); slots[id] = n; }
	}

	void run() {
		int32_t v1, v2; std::string op;
		std::cout << "Enter (A op B): "; std::cin >> v1 >> op >> v2;
		if (symbol_to_id.count(op)) {
			number res = slots[symbol_to_id[op]]->execute(number(v1), number(v2));
			std::cout << "Result: " << res.get_val() << "\n";
		}
		else std::cout << "Unknown op.\n";
	}
};

int main() {
	ALU alu;
	alu.load_all("memory.dat");
	while (true) {
		std::cout << "\n1. Learn (File)  2. Use ALU  3. Save & Exit\n> ";
		int c; std::cin >> c;
		if (c == 1) alu.learn_from_file();
		else if (c == 2) alu.run();
		else { alu.save_all("memory.dat"); break; }
	}
	return 0;
}