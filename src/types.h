#pragma once

#include <assert.h>
#include <float.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>

#define function static inline

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32   bool32;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef intptr_t intptr;
typedef uintptr_t uintptr;

typedef size_t size;

typedef float float32;
typedef double float64;

typedef unsigned char byte;

typedef struct str {
  size        count;
  const byte* buffer;
} str;

function size_t count_c_string(const char* c_string) {
  size_t count = 0;
  while (*c_string++) count++;
  return count;
}

function str str_from_c_string(const char* c_string) {
  str result = {
	.count = count_c_string(c_string),
	.buffer = (const byte*)c_string,
  };

  return result;
}

function void read_file_to_str(char* path, str* result) {
  int32 file_descriptor = open(path, O_RDONLY, 0);
  assert(file_descriptor != -1);

  struct stat file_status;
  if (stat(path, &file_status) == -1) {
	printf("Unable to get file status.");
  }

  byte* buffer = malloc(file_status.st_size);
  read(file_descriptor, buffer, file_status.st_size);
  close(file_descriptor);

  result->buffer = buffer;
  result->count = file_status.st_size;
}
