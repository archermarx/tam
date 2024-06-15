#ifndef TAM_VECTOR_INCLUDE_H
#define TAM_VECTOR_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef struct tam__vec_header {
  size_t len;
  size_t cap;
  char data[0];
} tam__vec_header;

#define tam__vec_hdr(v) ((tam__vec_header*)((char*)(v) - offsetof(tam__vec_header, data)))
#define tam__vec_fits(v, n) (tam_vec_len(v) + (n) <= tam_vec_cap(v))
#define tam__vec_fit(v, n) (tam__vec_fits((v), (n)) ? 0 : ((v) = tam__vec_grow((v), tam_vec_len(v) + (n), sizeof(*(v)))))

#define tam_vec_push(v, x) (tam__vec_fit((v), 1), (v)[tam__vec_hdr(v)->len++] = (x))
#define tam_vec_free(v) do {(v) ? (free(tam__vec_hdr(v)), (v) = NULL) : 0;} while (0)
#define tam_vec_len(v) ((v) ? tam__vec_hdr(v)->len : 0)
#define tam_vec_cap(v) ((v) ? tam__vec_hdr(v)->cap : 0)
#define tam_vec_new(type, len) (tam__vec_new((len), sizeof(type)))
#define tam_vec_end(v) ((v) + tam_vec_len(v))

void* tam__vec_grow(const void *vec, size_t new_len, size_t elem_size);
void *tam__vec_new(size_t len, size_t elem_size);

// Implementation
#define TAM_VECTOR_IMPLEMENTATION
#ifdef TAM_VECTOR_IMPLEMENTATION

// Includes for implementation
#include <assert.h>
#include <stdlib.h>

#ifndef TAM_MEMORY_IMPLEMENTATION
#define TAM_MEMORY_IMPLEMENTATION
#endif
#include <tam/memory.h>

#define TAM__VECTOR_BASE_ALLOC 8
#define TAM__VECTOR_GROW_FACTOR 1.618

#include <stdio.h>

void* tam__vec_grow(const void *vec, size_t new_len, size_t elem_size) {
  size_t new_cap = 1 + (size_t)(TAM__VECTOR_GROW_FACTOR * (double)tam_vec_cap(vec));
  if (new_cap < new_len) {
    new_cap = new_len;
  }
  if (new_cap < 8) {
    new_cap = 8;
  }
  assert(new_len <= new_cap);
  size_t new_size = offsetof(tam__vec_header, data) + new_cap*elem_size;
  tam__vec_header *new_header;
  if (vec) {
    new_header = tam_reallocate(tam__vec_hdr(vec), new_size);
  } else {
    new_header = tam_allocate(new_size);
    new_header->len = 0;
  }
  new_header->cap = new_cap;
  return new_header->data;
}

void *tam__vec_new(size_t len, size_t elem_size) {
  size_t cap = len;
  tam__vec_header *header;
  size_t size = offsetof(tam__vec_header, data) + cap * elem_size;
  header = tam_allocate(size);
  memset(header->data, 0, cap * elem_size);
  header->len = len;
  header->cap = cap;
  return header->data;
}

#endif // TAM_VECTOR_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif  // TAM_VECTOR_INCLUDE_H

