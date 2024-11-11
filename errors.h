#ifndef TAM_ERRORS_H
#define TAM_ERRORS_H

void tam_errorf(const char *fmt, ...);

#if defined(USING_NAMESPACE_TAM) || defined(USING_TAM_ERRORS)
#define errorf tam_errorf
#endif

#if defined(TAM_IMPLEMENTATION) || defined(TAM_ERROR_IMPLEMENTATION)

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <tam/colors.h>

void tam_errorf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  fprintf(stderr, BRED "Error: ");
  vfprintf(stderr, fmt, args);
  fprintf(stderr, CRESET "\n");

  va_end(args);
  exit(1);
}

#endif // TAM_ERROR_IMPLEMENTATION

#endif // TAM_ERROR_H
