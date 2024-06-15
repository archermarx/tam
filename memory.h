#ifndef TAM__MEMORY_H
#define TAM__MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

void *tam_reallocate(void *ptr, size_t num_bytes);
void *tam_allocate(size_t num_bytes);
void *tam_allocate_elems(size_t num_elements, size_t elem_size);
void *tam_zero_allocate_elems(size_t num_elements, size_t elem_size);

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

void *tam_allocate_elems(size_t num_elements, size_t elem_size) {
  void *ptr = malloc(num_elements * elem_size);
  if (!ptr) {
    fprintf(stderr, "allocation of %zu elements of size size %zu failed.\n", num_elements, elem_size);
  }
  return ptr;
}

void *tam_zero_allocate_elems(size_t num_elements, size_t elem_size) {
  void *ptr = calloc(num_elements, elem_size);
  if (!ptr) {
    fprintf(stderr, "zero-allocation of %zu elements of size size %zu failed.\n", num_elements, elem_size);
  }
  return ptr;
}

#endif // TAM_MEMORY_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // TAM__MEMORY_H
