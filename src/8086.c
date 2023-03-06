#include "types.h"
#include "decoder.h"
#include "decoder.c"

int32 main(int32 argc, char* argv[]) {
  if (argc == 1) {
	fprintf(stderr, "No path given.");
	return 1;
  }

  char* path = argv[1];
  str instruction_stream;
  read_file_to_str(path, &instruction_stream);
  /* printf("Instruction Stream: %s\nCount: %zu\n", instruction_stream.buffer, instruction_stream.count); */

  bool32 result = decode(instruction_stream);

  free((void*)instruction_stream.buffer);
  return result;
}
