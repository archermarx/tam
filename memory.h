#ifndef TAM__MEMORY_H
#define TAM__MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

void *tam_reallocate(void *ptr, size_t num_bytes);
void *tam_allocate(size_t num_bytes);

#ifdef TAM_MEMORY_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>

void *tam_reallocate(void *ptr, size_t num_bytes) {
  void *new_ptr = realloc(ptr, num_bytes);
  if (!new_ptr) {
    fprintf(stderr, "reallocation of size %zu at address %p failed.\n", num_bytes, ptr);
    exit(1);
  }
  ptr = NULL;
  return new_ptr;
}

void *tam_allocate(size_t num_bytes) {
  void* ptr = malloc(num_bytes);
  if (!ptr) {
    fprintf(stderr, "allocation of size %zu failed.\n", num_bytes);
  }
  return ptr;
}

#endif // TAM_MEMORY_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // TAM__MEMORY_H
