#include "types.h"

// OPs
#define MOV_register_or_memory_to_from_register 0b100010
#define MOV_immediate_to_register               0b1011

typedef enum Register {
	/* W = 0 */  AL, CL, DL, BL, AH, CH, DH, BH,
	/* W = 1 */  AX, CX, DX, BX, SP, BP, SI, DI,
} Register;

static char* register_name[] = {
	/* W = 0 */  "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh",
	/* W = 1 */  "ax", "cx", "dx", "bx", "sp", "bp", "si", "di",
};

typedef struct Decoder {
	str   instruction_stream;
	byte* position;
	byte* end;
} Decoder;

byte read_byte(Decoder *decoder) {
	/* assert(decoder->position < decoder->end); */
	return *decoder->position++;
}

Register decode_register(byte W, byte reg) {
	return (W << 3) | reg;
}

bool32 decode(str instruction_stream) {
	Decoder decoder = {
		.instruction_stream = instruction_stream,
		.position           = (byte*)instruction_stream.buffer,
		.end                = (byte*)instruction_stream.buffer + instruction_stream.count,
	};

	printf("bits 16\n");

	for (byte high_byte = read_byte(&decoder); decoder.position < decoder.end; high_byte = read_byte(&decoder)) {

		// 6bit prefixes
		byte op = high_byte >> 2;
		if (op == MOV_register_or_memory_to_from_register) {
			byte D = (high_byte >> 1) & 0x1;
			byte W =  high_byte       & 0x1;
			/* printf("high_byte: %o OP: %o W: %o D: %o\n", high_byte, op, W, D); */

			byte low_byte = read_byte(&decoder);

			byte MOD = (low_byte >> 6) & 0x3;
			byte REG = (low_byte >> 3) & 0x7;
			byte R_M = (low_byte >> 0) & 0x7;
			assert(MOD == 0x3); // We only support MOD = 11, i.e. MEM
			/* printf("low_byte: %o MOD: %o REG: %o R_M: %o\n", low_byte, MOD, REG, R_M); */

			Register target = decode_register(W, D ? REG : R_M);
			Register source = decode_register(W, D ? R_M : REG);
			/* printf("target: %o source: %o\n", target, source); */

			printf("MOV %s, %s\n", register_name[target], register_name[source]);

			continue;
		}

		// 4bit prefixes
		op = op >> 2;
		if (op == MOV_immediate_to_register) {
			byte W   = (high_byte >> 3) & 0x1;
			byte REG = high_byte        & 0x7;

			Register target = decode_register(W, REG);
			uint16 data = 0;

			if (W) {
				byte data_low  = read_byte(&decoder);
				byte data_high = read_byte(&decoder);
				data = data_low | (data_high << 8);
			} else {
				data = read_byte(&decoder);
			}

			printf("MOV %s, %i\n", register_name[target], data);
			continue;
		}

		printf("Decoder error, cannot find op for byte %o. Looked at op: %o", high_byte, op);
		assert(0);
	}

	return 0;
}
