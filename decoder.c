#include "types.h"

typedef enum OP {
  MOV = 0b100010,
} OP;

typedef enum Register {
  // W = 0
  AL = 0b0000,
  CL = 0b0001,
  DL = 0b0010,
  BL = 0b0011,
  AH = 0b0100,
  CH = 0b0101,
  DH = 0b0110,
  BH = 0b0111,

  // W = 1
  AX = 0b1000,
  CX = 0b1001,
  DX = 0b1010,
  BX = 0b1011,
  SP = 0b1100,
  BP = 0b1101,
  SI = 0b1110,
  DI = 0b1111,
} Register;

static char* register_name[] = {
  "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh",
  "ax", "cx", "dx", "bx", "sp", "bp", "si", "di",
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

  for (byte high_byte = read_byte(&decoder);
	   decoder.position < decoder.end;
	   high_byte = read_byte(&decoder)) {
	OP op = high_byte >> 2; // >> 2 discards the two rightmost bytes

	switch (op) {

	case MOV: {
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
	} break;

	default: printf("Decoder error!");
	}
  }

  return 0;
}
