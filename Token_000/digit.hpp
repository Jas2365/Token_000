_Pragma("once");
#include <iostream>
struct Digit {

	uint32_t char_code	: 16;
	uint32_t value		:  4;
	uint32_t reserved	: 12;
	
	Digit() = default;
	Digit(uint16_t char_code, uint32_t value) : char_code(char_code), value(value), reserved(0) {}
};

namespace Digits {
	
	inline const Digit d0('0', 0x0);
	inline const Digit d1('1', 0x1);
	inline const Digit d2('2', 0x2);
	inline const Digit d3('3', 0x3);
	inline const Digit d4('4', 0x4);
	inline const Digit d5('5', 0x5);
	inline const Digit d6('6', 0x6);
	inline const Digit d7('7', 0x7);
	inline const Digit d8('8', 0x8);
	inline const Digit d9('9', 0x9);
	inline const Digit da('a', 0xa);
	inline const Digit db('b', 0xb);
	inline const Digit dc('c', 0xc);
	inline const Digit dd('d', 0xd);
	inline const Digit de('e', 0xe);
	inline const Digit df('f', 0xf);

	inline const Digit all_digits[]{
		d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, da, db, dc, dd, de, df
	};
	

}; // namespace Digits

inline static void print_d(Digit d) {
	std::cout << char(d.char_code) << " : 0x" << std::hex << d.value << "\n";
}



