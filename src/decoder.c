#include "types.h"

// OPs
#define MOV_register_or_memory_to_from_register 0b100010
#define MOV_immediate_to_register               0b1011

typedef enum Register {
	/* W = 0 */  AL, CL, DL, BL, AH, CH, DH, BH,
	/* W = 1 */  AX, CX, DX, BX, SP, BP, SI, DI,
} Register;

static char* address_register_equation[] = {
	"bx + si", "bx + di", "bp + si", "bp + di", 
	"si",      "di",      "bp",      "bx"
};

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
		.end                = (byte*)instruction_stream.buffer + instruction_stream.count
	};

	printf("bits 16\n\n");

	for (byte high_byte = read_byte(&decoder);
		 decoder.position < decoder.end;
		 high_byte = read_byte(&decoder)) {

		// 6bit prefixes
		byte op = high_byte >> 2;
		if (op == MOV_register_or_memory_to_from_register) {
			byte D = (high_byte >> 1) & 0x1;
			byte W =  high_byte       & 0x1;

			byte low_byte = read_byte(&decoder);
			byte MOD =  low_byte >> 6;
			byte REG = (low_byte >> 3) & 0x7;
			byte R_M = low_byte        & 0x7;

			Register target = decode_register(W, D ? REG : R_M);
			Register source = decode_register(W, D ? R_M : REG);

			switch (MOD) {

			case 0b00: {
				printf("MOV %s, [%s]\n",
					   register_name[target],
					   address_register_equation[source & 0x7]); // We don't want the extra byte set by W.
			} break;

			case 0b01: {
				byte displacement = read_byte(&decoder);
				char* displacement_equation = "";
				int allocated_string_buffer = 0;

				if (displacement) {
					allocated_string_buffer = asprintf(&displacement_equation, " + %i", displacement);
					assert(allocated_string_buffer != -1);
				}

				printf("MOV %s, [%s%s]\n",
					   register_name[target],
					   address_register_equation[source & 0x7],
					   displacement_equation);

				if (allocated_string_buffer) free(displacement_equation);
			} break;

			case 0b10: {
				byte displacement_low  = read_byte(&decoder);
				byte displacement_high = read_byte(&decoder);
				uint16 displacement = (displacement_high << 8) | displacement_low;
				printf("MOV %s, [%s + %i]\n",
					   register_name[target],
					   address_register_equation[source & 0x7],
					   displacement);
			} break;

			case 0b11: {
				printf("MOV %s, %s\n", register_name[target], register_name[source]);
			} break;

			default: {
				printf("Unknown value of MOD: %o", MOD);
				assert(0);
			} break;

			}

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
