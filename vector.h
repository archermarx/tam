#ifndef TAM_VECTOR_INCLUDE_H
#define TAM_VECTOR_INCLUDE_H

#include <stddef.h>

typedef void* tam_vec;

// Free a vector
void tam_vec_free(tam_vec vec);

// Get the vector's length
size_t tam_vec_len(const tam_vec vec);

// Get the vector's capacity
size_t tam_vec_cap(const tam_vec vec);

// Implementation details for common vector utils
#define TAM_VECTOR_IMPLEMENTATION
#ifdef TAM_VECTOR_IMPLEMENTATION

#include <assert.h>
#include <stdlib.h>

#define TAMINT__VECTOR_BASE_ALLOC 8
#define TAMINT__VECTOR_GROW_FACTOR 1.618

// COMMON UTILITIES FOR ALL VECTORS
// Header info for vector
typedef struct tamint__vec_header {
  size_t len;
  size_t cap;
  size_t elem_size;
} tamint__vec_header;

static tam_vec tamint__vec_alloc(const size_t len, const size_t cap,
                                 const size_t size) {
  // Allocate memory for data + header
  tamint__vec_header* data = (tamint__vec_header*)calloc(
      cap + sizeof(tamint__vec_header) / size, size);

  // Write header info
  *data = (tamint__vec_header){.len = len, .cap = cap, .elem_size = size};

  // Increment past header to get to data
  tam_vec result = (tam_vec)(data + 1);

  return result;
}

static tamint__vec_header* tam_vec_header(const tam_vec vec) {
  return (tamint__vec_header*)vec - 1;
}

void tam_vec_free(tam_vec vec) { free(tam_vec_header(vec)); }
size_t tam_vec_cap(const tam_vec vec) { return tam_vec_header(vec)->cap; }
size_t tam_vec_len(const tam_vec vec) { return tam_vec_header(vec)->len; }

static tam_vec tam_vec_grow(tam_vec vec) {
  tamint__vec_header* header = tam_vec_header(vec);
  size_t size = header->elem_size;
  size_t new_cap = (size_t)(header->cap * TAMINT__VECTOR_GROW_FACTOR);
  header = realloc(header, new_cap * size + sizeof(tamint__vec_header));
  // zero memory
  header->cap = new_cap;
  char* new_vec = (char*)(header + 1);
  for (size_t i = header->len * size; i < new_cap * size; i++) {
    new_vec[i] = 0;
  }
  vec = (tam_vec)new_vec;
  return vec;
}

#endif  // TAM_VECTOR_IMPLEMENTATION

// Type-specific utilities

#define TAMINT__DECLARE_VECTOR_TYPE(name, type) \
  typedef type* name;                           \
  name name##_new(void);                        \
  name name##_newlen(size_t len);               \
  name name##_fill(size_t len, type elem);      \
  void name##_push(name vec, type elem);

#ifdef TAM_VECTOR_IMPLEMENTATION
#define TAMINT__DEFINE_VECTOR_TYPE(name, type)                   \
  name name##_new(void) {                                        \
    return (name)tamint__vec_alloc(0, TAMINT__VECTOR_BASE_ALLOC, \
                                   sizeof(type));                \
  }                                                              \
                                                                 \
  name name##_newlen(size_t len) {                               \
    return (name)tamint__vec_alloc(len, len, sizeof(type));      \
  }                                                              \
                                                                 \
  name name##_fill(size_t len, type elem) {                      \
    name vec = (name)tamint__vec_alloc(len, len, sizeof(type));  \
    for (size_t i = 0; i < len; i++) {                           \
      vec[i] = (type)elem;                                       \
    }                                                            \
    return vec;                                                  \
  }                                                              \
                                                                 \
  void name##_push(name vec, type elem) {                        \
    tamint__vec_header* header = tam_vec_header(vec);            \
    if (header->len >= header->cap) vec = tam_vec_grow(vec);     \
    vec[header->len] = elem;                                     \
    header->len += 1;                                            \
  }
#else
#define TAMINT__DEFINE_VECTOR_TYPE(name, type) ;
#endif  // TAM_VECTOR_IMPLEMENTATION

#define TAM_DECLARE_VECTOR_TYPE(name, type) \
  TAMINT__DECLARE_VECTOR_TYPE(name, type)   \
  TAMINT__DEFINE_VECTOR_TYPE(name, type)

#endif  // TAM_VECTOR_INCLUDE_H

