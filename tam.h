#ifndef TAM_INCLUDE_H
#define TAM_INCLUDE_H

// define all sub-implementations
#ifdef TAM_IMPLEMENTATION

#define TAM_STRING_IMPLEMENTATION
#define TAM_VECTOR_IMPLEMENTATION
#define TAM_MEMORY_IMPLEMENTATION

#endif  // TAM_IMPLEMENTATION

#include "string.h"
#include "vector.h"
#include "memory.h"

#endif  // TAM_INCLUDE_H
