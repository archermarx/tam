#ifndef TAM_MEMORY_H
#define TAM_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <tam/types.h>
// internals
void *TAM_allocate(usize num_elements, usize elem_size);
void *TAM_reallocate(void *ptr, usize num_elements, usize elem_size);
void TAM_deallocate(void *ptr);

// public interface
#define tam_allocate(type, count) (TAM_allocate((count), sizeof(type)))
#define tam_allocate_bytes(count) (TAM_allocate((count), sizeof(char)))
#define tam_reallocate(ptr, type, count) (TAM_reallocate((ptr), (count), sizeof(type)))
#define tam_reallocate_bytes(ptr, count) (TAM_reallocate((ptr), (count), sizeof(char)))
#define tam_deallocate(ptr) (TAM_deallocate(ptr), (ptr) = NULL)

// arena
typedef struct tam_arena_t {
  char *beg;
  char *ptr;
  isize cap;
} tam_arena_t;

#define tam_arena_alloc(a, t, n) (t *)TAM_arena_allocate(a, sizeof(t), _Alignof(t), n)
#define tam_arena_realloc(a, p, t, n1, n2) (t *)TAM_arena_reallocate(a, p, sizeof(t), _Alignof(t), n1, n2)
#define tam_arena_grow_arr(a, p, t, n1, n2) (t *)TAM_arena_grow_arr(a, p, sizeof(t), _Alignof(t), n1, n2)

tam_arena_t tam_arena_new(isize cap);
void *TAM_arena_allocate(tam_arena_t *a, isize size, isize align, isize count);
void *TAM_arena_reallocate(tam_arena_t *a, void *ptr, isize size, isize align, isize count, isize new_count);
void *TAM_arena_grow_arr(tam_arena_t *a, void *arr, isize size, isize align, isize count, isize *new_count);
void tam_arena_dealloc(tam_arena_t *arena);

#if defined(USING_NAMESPACE_TAM) || defined(USING_TAM_MEMORY) ///{{{
#define allocate tam_allocate
#define allocate_bytes tam_allocate_bytes
#define reallocate tam_reallocate
#define reallocate_bytes tam_reallocate_bytes
#define deallocate tam_deallocate
typedef tam_arena_t arena_t;
#define arena_new tam_arena_new
#define arena_alloc tam_arena_alloc
#define arena_realloc tam_arena_realloc
#define arena_dealloc tam_arena_dealloc
#define arena_grow_arr tam_arena_grow_arr
#endif // namespace }}}

#if defined(TAM_IMPLEMENTATION) || defined(TAM_MEMORY_IMPLEMENTATION) ///{{{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tam/errors.h>

void *TAM_allocate(usize num_elements, usize elem_size) {
  void *ptr = calloc(num_elements, elem_size);
  if (ptr == NULL)
    tam_errorf("allocation of %zu elements of size size %zu failed.", num_elements, elem_size);
  return ptr;
}

void *TAM_reallocate(void *ptr, usize num_elements, usize elem_size) {
  ptr = realloc(ptr, num_elements * elem_size);
  if (ptr == NULL)
    tam_errorf(stderr, "reallocation of %zu elements of size size %zu failed.", num_elements, elem_size);
  return ptr;
}

void TAM_deallocate(void *ptr) {
  if (ptr == NULL)
    return;
  free(ptr);
}

tam_arena_t tam_arena_new(isize cap) {
  tam_arena_t arena = {.beg = NULL, .cap = cap, .ptr = NULL};
  return arena;
}

void *TAM_arena_allocate(tam_arena_t *a, isize size, isize align, isize count) {
  if (a->beg == NULL) {
    a->beg = tam_allocate(char, a->cap);
    a->ptr = a->beg;
  }

  char *end = a->beg + a->cap;
  isize alloc = count * size;
  isize padding = -(uintptr_t)a->ptr & (align - 1);
  isize available = end - a->ptr - padding;
  if (available < 0 || count > available / size) {
    tam_errorf("arena allocation of size %lu failed -- out of memory", alloc);
  }
  void *p = a->ptr + padding;
  a->ptr += padding + alloc;
  return p;
}

void *TAM_arena_reallocate(tam_arena_t *a, void *ptr, isize size, isize align, isize count, isize new_count) {
  if (new_count <= count) {
    return ptr;
  }
  void *new_ptr = TAM_arena_allocate(a, size, align, new_count);
  memcpy(new_ptr, ptr, count * size);
  return new_ptr;
}

#define DEFAULT_ARRAY_SIZE 8
void *TAM_arena_grow_arr(tam_arena_t *a, void *arr, isize size, isize align, isize count, isize *new_count) {
  usize new_cap;
  if (arr == NULL || count == 0) {
    new_cap = DEFAULT_ARRAY_SIZE;
  } else {
    new_cap = count * 2;
  }
  if (new_count) {
    *new_count = new_cap;
  }
  void *new_ptr = TAM_arena_reallocate(a, arr, size, align, count, new_cap);
  return new_ptr;
}

void tam_arena_dealloc(tam_arena_t *a) {
  tam_deallocate(a->beg);
  a->beg = NULL;
  a->ptr = NULL;
  a->cap = 0;
}

#endif // TAM_MEMORY_IMPLEMENTATION }}}

#ifdef __cplusplus
}
#endif

#endif // TAM__MEMORY_H
