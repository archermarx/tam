#ifndef TAM_MEMORY_H
#define TAM_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

// internals
void *TAM_allocate(size_t num_elements, size_t elem_size);
void *TAM_reallocate(void *ptr, size_t num_elements, size_t elem_size);
void TAM_deallocate(void *ptr);

// public interface
#define tam_allocate(type, count) \
    (TAM_allocate((count), sizeof(type)))

#define tam_allocate_bytes(count) \
    (TAM_allocate((count), sizeof(char)))

#define tam_reallocate(ptr, type, count) \
    (TAM_reallocate((ptr), (count), sizeof(type)))    

#define tam_reallocate_bytes(ptr, count) \
    (TAM_reallocate((ptr), (count), sizeof(char)))    

#define tam_deallocate(ptr) \
    (TAM_deallocate(ptr), (ptr) = NULL)

#if defined (USING_NAMESPACE_TAM) || defined(USING_TAM_MEMORY) ///{{{
#define allocate         tam_allocate
#define allocate_bytes   tam_allocate_bytes
#define reallocate       tam_reallocate
#define reallocate_bytes tam_reallocate_bytes
#define deallocate       tam_deallocate
#endif // namespace }}} 

#if defined(TAM_IMPLEMENTATION) || defined(TAM_MEMORY_IMPLEMENTATION) ///{{{

#include <stdio.h>
#include <stdlib.h> 

void *TAM_allocate(size_t num_elements, size_t elem_size) {
    void *ptr = calloc(num_elements, elem_size);
    if (!ptr) {
        fprintf(stderr, "allocation of %zu elements of size size %zu failed.\n", num_elements, elem_size);
    }
    return ptr;
}

void *TAM_reallocate(void *ptr, size_t num_elements, size_t elem_size) {
    ptr = realloc(ptr, num_elements * elem_size);
    if (!ptr) {
        fprintf(stderr, "reallocation of %zu elements of size size %zu failed.\n", num_elements, elem_size);
        exit(1);
    }
    return ptr;
}

void TAM_deallocate(void *ptr) {
    if (ptr == NULL) return;
    free(ptr);
}

#endif // TAM_MEMORY_IMPLEMENTATION }}}

#ifdef __cplusplus
}
#endif

#endif // TAM__MEMORY_H
