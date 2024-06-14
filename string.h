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
 * Create a new string from a char* range
 */
tam_str tam_substr(const char* start, const char* end);

/**
 * Duplicate a string
 */
tam_str tam_strdup(const tam_str s);

/**
 * Get length of a string
 */
size_t tam_strlen(const tam_str s);

/**
 * Free a string
 */
void tam_strfree(tam_str s);

/**
 * Concatenate a string to a char*, allocating a new string to hold the result
 * This will work for a tam_str as well, but calls strlen to get the length.
 * To avoid this, use `tam_strcatstr`.
 */
tam_str tam_strcat(const tam_str left, const char* right);

/**
 * Concatenate a string to another tam_str.
 * This will likely segfault if passed a normal char *, and will definitely
 * produce unexpected results. To avoid this, use `tam_strcat`.
 */
tam_str tam_strcatstr(const tam_str left, const tam_str right);

/**
 * Check if a string equals another string
 */
int tam_strcmp(const tam_str left, const tam_str right);

#ifdef __cplusplus
}
#endif

// Implementation section
#ifdef TAM_STRING_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

// String header
// Currently just contains length, but could in principle contain other data
typedef struct tam__strheader {
  size_t len;
} tam__strheader;

static tam_str tam__stralloc(const size_t len) {
  // memory to allocate
  // store length and capacity before allocation
  // Store room for null terminator as well
  size_t header_bytes = sizeof(tam__strheader) / sizeof(char);
  size_t bytes_to_alloc = header_bytes + len + 1;

  // Explicit cast for c++ compatability
  // calloc zeros memory so we don't need to explicitly add null-terminator
  char* strdata = (char*)calloc(bytes_to_alloc, sizeof(char));

  // write header data
  tam__strheader* header = (tam__strheader*)strdata;
  header[0] = (tam__strheader){.len = len};

  // Increment pointer past capacity and length to start of string
  tam_str result = strdata + header_bytes;

  // Return empty buffer
  return result;
}

static void tam__strncpy(tam_str dst, const char* src, const size_t n) {
  // Fill string data
  for (size_t i = 0; i < n; i++) {
    dst[i] = src[i];
  }
}

static tam_str tam__strnewlen(const char* s, const size_t len) {
  // Allocate memory for string and fill header
  tam_str result = tam__stralloc(len);
  tam__strncpy(result, s, len);
  return result;
}

tam_str tam_substr(const char* start, const char* end) {
  // Allocate memory for string and fill header
  size_t len = (size_t)(end - start);
  return tam__strnewlen(start, len);
}

tam_str tam_strnew(const char* s) {
  // get length of input string
  size_t len = strlen(s);

  // Return string
  return tam__strnewlen(s, len);
}

tam_str tam_strdup(const tam_str s) {
  size_t len = tam_strlen(s);
  return tam__strnewlen(s, len);
}

// Get the header location
static tam__strheader* tam__getstrheader(const tam_str s) {
  return (tam__strheader*)s - 1;
}

size_t tam_strlen(const tam_str s) {
  tam__strheader* header = tam__getstrheader(s);
  return header->len;
}

void tam_strfree(tam_str s) { free(tam__getstrheader(s)); }

tam_str tam_strcat(const tam_str left, const char* right) {
  size_t left_len = tam_strlen(left);
  size_t right_len = strlen(right);
  tam_str result = tam__stralloc(left_len + right_len);
  tam__strncpy(result, left, left_len);
  tam__strncpy(result + left_len, right, right_len);
  return result;
}

#include <stdio.h>

tam_str tam_strcatstr(const tam_str left, const tam_str right) {
  size_t left_len = tam_strlen(left);
  size_t right_len = tam_strlen(right);

  tam_str result = tam__stralloc(left_len + right_len);
  tam__strncpy(result, left, left_len);
  tam__strncpy(result + left_len, right, right_len);
  return result;
}

int tam_strcmp(const tam_str left, const tam_str right) {
  size_t len = tam_strlen(left);
  int result = 1;
  if (len == tam_strlen(right)) {
    for (size_t i = 0; i < len; i++) {
      result = result && (left[i] == right[i]);
    }
  } else {
    return 0;
  }
}

#endif  // INCLUDE_TAM_STRING_H
#endif  // TAM_STRING_IMPLEMENTATION

