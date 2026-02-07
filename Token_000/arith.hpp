_Pragma("once");

#include <cstdint>

enum arith_op { add_op, sub_op, mul_op, div_op };

struct arith {

	uint32_t char_code : 16;
	uint32_t op : 2;
	uint32_t reserved : 14;
	
	arith() = default;
	arith(uint16_t char_code, arith_op op) : char_code(char_code), op(op), reserved(0) {}
};

namespace Arith {

	arith plus('+', arith_op::add_op);
	arith minus('-', arith_op::sub_op);
	arith multiply('*', arith_op::mul_op);
	arith divide('/', arith_op::div_op);

} // namespace Arith