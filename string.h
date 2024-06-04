#ifndef INCLUDE_TAM_STRING_H
#define INCLUDE_TAM_STRING_H

// String library, takes heavy inspiration from SDS
// https://github.com/antirez/sds

// Header section
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/**
 * Typedef for char*
 */
typedef char* tam_str;

/**
 * Create a new string from a char*
 */
tam_str tam_strnew(const char* s);

/**
 * Get length of a string
 */
size_t tam_strlen(const tam_str s);

/**
 * Free a string
 */
void tam_strfree(tam_str s);

#ifdef __cplusplus
}
#endif

// Implementation section
#ifdef TAM_STRING_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

// String header
// Currently just contains length, but could in principle contain other data
typedef struct tamint__strheader {
  size_t len;
} tamint__strheader;

tam_str tam_strnew(const char* s) {
  // get length of input string
  size_t len = strlen(s);

  // memory to allocate
  // store length and capacity before allocation
  // Store room for null terminator as well
  size_t header_bytes = sizeof(tamint__strheader) / sizeof(char);
  size_t bytes_to_alloc = header_bytes + len + 1;

  // Explicit cast for c++ compatability
  // calloc zeros memory so we don't need to explicitly add null-terminator
  char* strdata = (char*)calloc(bytes_to_alloc, sizeof(char));

  // write header data
  tamint__strheader* header = (tamint__strheader*)strdata;
  header[0] = (tamint__strheader){.len = len};

  // Increment pointer past capacity and length to start of string
  tam_str result = header_bytes + strdata;

  // Fill string data
  for (size_t i = 0; i < len; i++) {
    result[i] = s[i];
  }

  // Return string
  return result;
}

// Get the header location
static tamint__strheader* tamint__getstrheader(const tam_str s) {
  return (tamint__strheader*)s - 1;
}

size_t tam_strlen(const tam_str s) {
  tamint__strheader* header = tamint__getstrheader(s);
  return header->len;
}

void tam_strfree(tam_str s) { free(tamint__getstrheader(s)); }

#endif  // INCLUDE_TAM_STRING_H
#endif  // TAM_STRING_IMPLEMENTATION

