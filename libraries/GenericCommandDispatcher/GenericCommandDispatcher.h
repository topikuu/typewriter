#ifndef GCD_HEADER_GUARD_
#define GCD_HEADER_GUARD_

#include "Arduino.h"

template <typename OpcodeType, typename DataType, size_t N>
class GenericCommandDispatcher {
public:
	struct Command {
		Command(OpcodeType o, DataType d) :
			opcode(o),
			data(d)
		{}
		OpcodeType opcode;
		DataType data;
	};
	typedef void(*OpFunc)(DataType);

	void registerOp(OpcodeType opcode, OpFunc op) {
		m_ops[opcode] = op;
	}

	void dispatch(Command command) {
		m_ops[command.opcode](command.data);
	}
private:
	OpFunc m_ops[N];
};

#endif /* end of include guard: GCD_HEADER_GUARD_ */
