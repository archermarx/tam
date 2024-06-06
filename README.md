# TAM single-file libraries

## Contents:

`tam.h`: includes all libraries below

`string.h`: a basic dynamic string, inspired by [`sds`](https://github.com/antirez/sds)

`vector.h`: a basic dynamic array, implemented similarly to the string. Supports multiple types.

## Usage:
Include the libraries in your project as normal.
In one (and only one) source file, you must create a `#define` to instantiate the implementation, as shown below.

```c
#define TAM_<LIB>_IMPLEMENTATION
#include <tam/lib.h>

// your code goes here
```

For example, if using `tam/string.h` you would `#define TAM_STRING_IMPLEMENTATION`.

Alternatively, you can include `tam/tam.h` and `#define TAM_IMPLEMENTATION` to include and instantiate all of the libraries.
