#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>

#include "types.h"

bool32 decode_asm_file(char* path) {
  int32 asm_file = open(path, O_RDONLY, 0);
  if (asm_file == -1) {
	printf("No file found in path %s", path);
	return -1;
  }

  struct stat asm_file_status;
  if (stat(path, &asm_file_status) == -1) {
	printf("Unable to get file status.");
	return -1;
  }

  char* instruction_stream = malloc(asm_file_status.st_size);
  read(asm_file, instruction_stream, asm_file_status.st_size);

  printf("Instruction Stream: %s\n", instruction_stream);

  free(instruction_stream);
  close(asm_file);
  return 0;
}

int32 main(int32 argc, char* argv[]) {
  if (argc == 1) {
	printf("No path given");
	return -1;
  }

  char* path = argv[1];
  bool32 result = decode_asm_file(path) ;

  return result;
}
