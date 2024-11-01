#ifndef TAM_STRINGS_H
#define TAM_STRINGS_H

// TAM String utility library
// Copyright 2024 Thomas Archer Marks
//
// Contains two main objects - a Slice and a StringBuilder
// The former is a non-owning view on a char* that stores a length
// The latter is a linked list of such views that can be used to 
// construct a string from many smaller strings.
//
// TODO: owning slices, or a different type that acts very similarly
// that owns its memory.
//
// String could either reimplement many of slice's methods, or just
// require you to cast to a slice.
// On top of slice's methods, it would also support resizing
// (probably just appending for now).
// Since String would own memory, constructing one would always
// trigger a new heap allocation.
// Strings would also need to be freed.
// Casting between them doesn't seem to be possible, since we'd need to
// either store a capacity along with the string or ensure len = cap
// Instead, provide a function sslice (or make slice generic?)
// For the rest, try _Generic, with the String versions just creating a
// slice and passing to the corresponding slice version
// a generic indexing function (s_idx) would be good.
//
// TODO: things like join, so you can do something like
// sb_join(const char** elems, size_t num_elems, char* pat);
//
// const char* elems[] = {"some", "comma", "separated", "values"};
// const char* result = sb_join(elems, 4, ",");
// -- result = "some,comma,separated,values"
//
// However, since we know ahead of time the sizes and number,
// it might just be better to allocate everything in one go and return
// one of these hypothetical "owning strings".

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

//*** ## String declarations *** {{{

typedef struct tam_string_t {
    size_t len;
    size_t cap;
    char *buf;
} tam_string_t;

tam_string_t tam_string(const char *buf);
tam_string_t tam_string_n(const char *buf, size_t n);

#if defined(USING_NAMESPACE_TAM) || defined(USING_TAM_STRINGS) //{{{
                                                               //
typedef tam_string_t String;
#define string       tam_string();
#define string_n     tam_string_n();

#endif // end String namespacing }}}

// ## end String declarations }}}

//*** ## Slice declarations *** {{
/*
 * These are non-owning views on string data that store a pointer to the underlying data
 * as well as a length. These are convenient for any string manipulation tasks that do not
 * require that the underlying data is changed or any new memory to be allocated.
 * The underlying char data is expected to be null-terminated, as we use libc string functions
 * for many utility functions.
 * 
 * A note on naming conventions -- functions which create slices are all prefixed by `slice_`,
 * while those that operate on slices but return some other type are prefixed by `sl_`.
 * One exception is `reslice`, which creates a slice but has neither of these prefixes.
 * Additionally, it can always be assumed that any functions taking in a slice pointer (tam_slice_t*)
 * will modify their input, so act accordingly.
 */
typedef struct tam_slice_t {
    int64_t len;
    const char *buf;
} tam_slice_t;

//*** ### Slice construction *** {{{

/*
 * Obtain the character at index i in a slice.
 * This does not return a pointer to the character, so it cannot be used to modify the underlying memory.
 */
char tam_sl_idx(tam_slice_t s, int64_t i);

/*
 * Construct a slice from a raw char* buf.
 * This calls strlen to get the length, so `buf` must be null-terminated.
 * If you have already computed the length or otherwise do want to rely on `strlen`, use `slice_len` below.
 */
#define tam_slice(buf) (tam_slice_t){strlen(buf), buf}

/*
 * Construct a slice from a raw char* buf and a length.
 */
#define tam_slice_n(buf, len) (tam_slice_t){len, buf}

/*
 * Construct a slice from a String.
 */
#define tam_slstr(str) (tam_slice_t){str.len, str.buf}

/*
 * Construct a slice from an existing slice and a start and stop index
 * We follow python and go conventions here, so the characters obtained are [i, j)
 */
tam_slice_t tam_reslice(tam_slice_t s, int64_t i, int64_t j);

/*
 * Construct a subslice using the implicit indices [0, i)
 * Equivalent to s[:i] in python
 */
tam_slice_t tam_slice_prefix(tam_slice_t s, int64_t i);

/*
 * Construct a subslice using the implicit indices [i, len(s))
 * Equivalent to s[i:] in python
 */
tam_slice_t tam_slice_suffix(tam_slice_t s, int64_t i);

// ### end slice construction }}}

//*** ### Slice utility functions *** {{{

/*
 * Check for literal equivalence between two slices,
 * i.e. whether they point to the same memory and have the same length.
 */
bool tam_sl_eqv(tam_slice_t s1, tam_slice_t s2);

/*
 * Check if two slices are equal on a byte-by-byte comparison.
 * They may have different addresses.
 */
bool tam_sl_eq(tam_slice_t s1, tam_slice_t s2);

/*
 * Check if the data contained in a slice is equal to a char*
 */
bool tam_sl_eqstr(tam_slice_t s, const char *c);

/*
 * NOTE: modifies the input slice!!!
 * Remove leading whitespace from a slice.
 * Return index of first char after leading spaces in original slice.
 */
int64_t tam_sl_lstrip(tam_slice_t *s);

/*
 * Create a slice by stripping leading whitespace from input slice
 */
tam_slice_t tam_slice_lstrip(tam_slice_t s);

/*
 * NOTE: modifies the input slice!!!
 * Remove trailing whitespace from a slice.
 * Return the index of first trailing space in original slice.
 */
int64_t tam_sl_rstrip(tam_slice_t *s);

/*
 * Create a slice by stripping trailing whitespace from input slice.
 */
tam_slice_t tam_slice_rstrip(tam_slice_t s);

/*
 * NOTE: modifies the input slice!!!
 * Remove leading and trailing whitespace from a slice.
 * Return the number of bytes removed.
 */
int64_t tam_sl_strip(tam_slice_t *s);

/*
 * Create a slice by stripping leading and trailing whitespace from input slice.
 */
tam_slice_t tam_slice_strip(tam_slice_t s);

/*
 * Scan a slice, looking for first occurrance of any bytes that are part of `reject`.
 * Return the number of bytes read before the first occurance.
 * Return the length of the string if no bytes in `reject` is not found. 
 */
int64_t tam_sl_cspan(tam_slice_t s, const char *reject);

/*
 * Scan a slice, looking for first occurance of any bytes not in `accept`.
 * Return the number of bytes read before the first occurrance.
 * Return the length of the string if all bytes are found in `accept`.
 */
int64_t tam_sl_span(tam_slice_t s, const char *accept);

/*
 * NOTE: modifies the input slice!!!
 * Scan a slice until the first byte contained in `delimiters` is reached.
 * Then, return a slice of all bytes read up until that point.
 * Finally, strip any delimiter bytes from the front of the input slice.
 */
tam_slice_t tam_slice_tok(tam_slice_t *s, const char *delimiters);

/*
 * Return true if a slice starts with a given string, and false otherwise
 */
bool tam_sl_startswith(tam_slice_t s, tam_slice_t x);

/*
 * Return true if a slice starts with a given string, and false otherwise
 */
bool tam_sl_startswithstr(tam_slice_t s, const char *str);

/*
 * Find index of first occurrance of slice `needle` in slice `haystack`
 * Returns length of `haystack` if `needle` not found
 */
int64_t tam_sl_find(tam_slice_t haystack, tam_slice_t needle);

/*
 * Find index of first occurrance of string `needle` in slice `haystack`
 * Returns length of `haystack` if `needle` not found.
 * NOTE: `sl_findstr` first calls strlen on `needle`. To avoid this, use
 * `sl_findstrn`
 */
#define tam_sl_findstr(haystack, needle) \
    (tam_sl_find(haystack, tam_slice(needle)))
#define tam_sl_findstrn(haystack, needle, needle_len) \
    (tam_sl_find(haystack, tam_slice_n(needle, needle_len)))

/*
 * NOTE: modifies the input slice!!!
 * Read a line from the input slice, stripping newlines
 */
#define tam_slice_getline(s) (tam_slice_tok(s, "\r\n"))
// ### end slice utility functions }}}

#if defined(USING_NAMESPACE_TAM) || defined(USING_TAM_STRINGS) //{{{

#define Slice tam_slice_t

// indexing
#define sl_idx tam_sl_idx

// construction
#define slice        tam_slice
#define slstr        tam_slstr
#define slice_n      tam_slice_n
#define reslice      tam_reslice
#define slice_prefix tam_slice_prefix
#define slice_suffix tam_slice_suffix

// utility functions
#define sl_eqv tam_sl_eqv
#define sl_eq tam_sl_eq
#define sl_eqstr tam_sl_eqstr
#define sl_lstrip tam_sl_lstrip
#define slice_lstrip tam_slice_lstrip
#define sl_rstrip tam_sl_rstrip
#define slice_rstrip tam_slice_rstrip
#define sl_strip tam_sl_strip
#define slice_strip tam_slice_strip
#define sl_cspan tam_sl_cspan 
#define sl_span tam_sl_span
#define slice_tok tam_slice_tok
#define sl_startswith tam_sl_startswith
#define sl_starswithstr tam_sl_startswithstr
#define sl_find tam_sl_find
#define sl_findstr tam_sl_findstr
#define sl_findstrn tam_sl_findstrn
#define slice_getline tam_slice_getline

#endif // end slice namespacing }}}

// end Slice declarations}}} 

//*** ## StringBuilder declarations *** {{{
/*
 * String builders are constructed using the sb_new function. 
 * They can then can be appended to repeatedly using `sb_append[slice/chars/charsn]`.
 * A string builder can then construct a string (char*) by calling `sb_tochars` on the builder.
 * The returned char* is heap-allocated and must be freed by the user.
 * The StrinBbuilder copies all data passed to it into an internal buffer, so the user is free to do whatever
 * they want with the data passed to the stringbuilder after they have done so.
 * The user is responsible for freeing the StringBuilder using sb_deallocate when they are done with it.
 */

typedef struct tam_stringbuilder_t {
    char *buf;
    size_t len;
    size_t cap;
} tam_stringbuilder_t;

/*
 * Create an empty StringBuilder
 */
tam_stringbuilder_t tam_sb_new();

/*
 * Free a StringBuilder instance
 */ 
void tam_sb_deallocate(tam_stringbuilder_t *sb);

/*
 * Append a slice to a StringBuilder
 */
void tam_sb_appendslice(tam_stringbuilder_t *sb, tam_slice_t sl);

/*
 * Append a char array to a StringBuilder
 */
void tam_sb_appendchars(tam_stringbuilder_t *sb, const char* s);

/*
 * Append a char array with known length to a StringBuilder
 */
void tam_sb_appendcharsn(tam_stringbuilder_t *sb, const char *s, size_t n);

/*
 * Append a formatted string to StringBuilder
 */
void tam_sb_appendf(tam_stringbuilder_t *sb, const char *fmt, ...);

/*
 * Construct a char array from a Stringbuilder
 */
char *tam_sb_tochars(tam_stringbuilder_t sb);

#if defined(USING_NAMESPACE_TAM) || defined (USING_TAM_STRINGS) ///{{{
    typedef tam_stringbuilder_t StringBuilder;
    #define sb_new              tam_sb_new
    #define sb_deallocate       tam_sb_deallocate
    #define sb_appendslice      tam_sb_appendslice
    #define sb_appendchars      tam_sb_appendchars
    #define sb_appendcharsn     tam_sb_appendcharsn
    #define sb_appendf          tam_sb_appendf
    #define sb_tochars          tam_sb_tochars
#endif // end StringBuilder namespace }}}

// end StringBuilder declarations }}}

//=======================================================================
//                          IMPLEMENTATIONS
//=======================================================================

#if defined(TAM_IMPLEMENTATION) || defined(TAM_STRINGS_IMPLEMENTATION)

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <tam/memory.h>
                                  
// ### Slice implementation {{{
/*
 * Helper function to convert an index i into one that can be used in a slice.
 * This handles negative indices to allow python-style wraparound indexing,
 * Negative indices count backward from the end of the array (-n -> len - abs(n))
 */
int64_t tam_get_slice_index(int64_t i, int64_t len) {
    int64_t j = i >= 0 ? i : len + i;
    assert(0 <= j && j <= len);
    return j;
}

char tam_sl_idx(tam_slice_t s, int64_t i) {
    return s.buf[tam_get_slice_index(i, s.len)];
}

tam_slice_t tam_reslice(tam_slice_t s, int64_t i, int64_t j) {
    i = tam_get_slice_index(i, s.len);
    j = tam_get_slice_index(j, s.len);
    assert(i <= j);
    return tam_slice_n(s.buf + i, j - i);
}

tam_slice_t tam_slice_prefix(tam_slice_t s, int64_t i) {
    return tam_slice_n(s.buf, tam_get_slice_index(i, s.len));
}

tam_slice_t tam_slice_suffix(tam_slice_t s, int64_t i) {
    int64_t j = tam_get_slice_index(i, s.len);
    return tam_slice_n(s.buf + j, s.len - j);
}

bool tam_sl_eqv(tam_slice_t s1, tam_slice_t s2) {
    return s1.buf == s2.buf && s1.len == s2.len;
}

bool tam_sl_eq(tam_slice_t s1, tam_slice_t s2) {
    if (s1.len != s2.len) return false;
    if (s1.buf == s2.buf) return true;
    return strncmp(s1.buf, s2.buf, s1.len) == 0;
}

bool tam_sl_eqstr(tam_slice_t s, const char *c) {
    return strncmp(s.buf, c, s.len) == 0;
}

int64_t tam_sl_lstrip(tam_slice_t *s) {
    int64_t i;
    for (i = 0; i < s->len && isspace(tam_sl_idx(*s, i)); i++);
    *s = tam_slice_suffix(*s, i);
    return i;
}

tam_slice_t tam_slice_lstrip(tam_slice_t s) {
    tam_sl_lstrip(&s);
    return s;
}

int64_t tam_sl_rstrip(tam_slice_t *s) {
    int64_t i;
    for (i = s->len - 1; i >= 0 && isspace(tam_sl_idx(*s, i)); i--);
    i++;
    *s = tam_slice_prefix(*s, i);
    return i;
}

tam_slice_t tam_slice_rstrip(tam_slice_t s) {
    tam_sl_rstrip(&s);
    return s;
}

int64_t tam_sl_strip(tam_slice_t *s) {
    int64_t orig_len = s->len;
    tam_sl_lstrip(s);
    tam_sl_rstrip(s);
    return orig_len - s->len;
}

tam_slice_t tam_slice_strip(tam_slice_t s) {
    tam_sl_strip(&s);
    return s;
}

int64_t tam_sl_cspan(tam_slice_t s, const char *reject) {
    int64_t idx = strcspn(s.buf, reject);
    if (idx > s.len) idx = s.len;
    return idx;
}

int64_t tam_sl_span(tam_slice_t s, const char *accept) {
    int64_t idx = strspn(s.buf, accept);
    if (idx > s.len) idx = s.len;
    return idx;
}

tam_slice_t tam_slice_tok(tam_slice_t *s, const char *delimiters) {
    int64_t prefix_size = tam_sl_cspan(*s, delimiters);
    tam_slice_t token = tam_slice_prefix(*s, prefix_size);
    tam_slice_t suffix = tam_slice_suffix(*s, prefix_size);
    
    assert(token.len + suffix.len == s->len);

    int64_t dlm_size = tam_sl_span(suffix, delimiters);
    *s = tam_slice_suffix(suffix, dlm_size);
    return token;
}

bool tam_sl_startswith(tam_slice_t s, tam_slice_t x) {
    return strncmp(x.buf, s.buf, x.len) == 0;
}

bool tam_sl_startswithstr(tam_slice_t s, const char *str) {
    return strncmp(str, s.buf, strlen(str)) == 0;
}

int64_t tam_sl_find(tam_slice_t haystack, tam_slice_t needle) {
    int64_t pos;
    // haystack:     "Hello, world!"
    // haystack inds: 0123456789ABC 
    // haystack.len: 13
    // if needle is "world!", then we need to advance the start pos up until 7
    // the needle length is 6, so we want to start the search at 7 = haystack.len - needle.len 
    for (pos = 0; pos <= (haystack.len - needle.len); pos++) {
        int i;
        for (i = 0; i < needle.len; i++) {
            if (tam_sl_idx(needle, i) != tam_sl_idx(haystack, pos+i)) {
                break;
            }
        }
        if (i == needle.len) return pos;
    }
    return haystack.len;
}
// end Slice implementation }}}

// ### StringBuilder implementation {{{

#define TAM_SB_INITIAL_CAPACITY 16

tam_stringbuilder_t tam_sb_new() {
    return (tam_stringbuilder_t){.cap=0, .len=0, .buf=NULL};
}

void tam_sb_deallocate(tam_stringbuilder_t *sb) {
    tam_deallocate(sb->buf);
    sb->cap = 0;
    sb->len = 0;
}

static void tam_sb_grow(tam_stringbuilder_t *sb, size_t newlen) {
    if (newlen <= sb->cap) return; 
    size_t newcap = sb->buf == NULL ? TAM_SB_INITIAL_CAPACITY : 2 * sb->cap;
    if (newcap < newlen) newcap = newlen+1;
    sb->buf = tam_reallocate(sb->buf, char, newcap);
    sb->cap = newcap;
}

// Appending
void tam_sb_appendcharsn(tam_stringbuilder_t *sb, const char *s, size_t n) {
    size_t newlen = sb->len + n;
    tam_sb_grow(sb, newlen);
    memcpy(sb->buf + sb->len, s, n);
    sb->len = newlen;
}

void tam_sb_appendslice(tam_stringbuilder_t *sb, tam_slice_t sl) {
    tam_sb_appendcharsn(sb, sl.buf, sl.len);
}

void tam_sb_appendchars(tam_stringbuilder_t *sb, const char *s) {
    tam_sb_appendcharsn(sb, s, strlen(s));
}

void tam_sb_appendf(tam_stringbuilder_t *sb, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    // first, determine size
    int size = vsnprintf(NULL, 0, fmt, va);
    va_end(va);

    // Next, allocated needed memory
    // need to restart va_list as it becomes invalidated after vsnprintf
    va_start(va, fmt);
    tam_sb_grow(sb, sb->len + size + 1);
    // finally, print formatted string to newly-allocated memory and increment length
    vsprintf(sb->buf + sb->len, fmt, va);
    sb->len += size;

    va_end(va);
}

// string creation
char *tam_sb_tochars(tam_stringbuilder_t sb) {
    char *buf = tam_allocate(char, sb.len + 1);
    memcpy(buf, sb.buf, sb.len * sizeof(char));
    return buf;
}

// end StringBuilder implementation }}}

#if defined(TAM_INCLUDE_TESTS) || defined(TAM_STRINGS_TEST)
// ### Slice tests {{{
int tam_test_slices() {
    {
        tam_slice_t s1 = tam_slice("Hello, world!");
        assert(tam_sl_idx(s1, 0) == 'H');
        assert(tam_sl_idx(s1, 1) == 'e');
        assert(tam_sl_idx(s1, -1) == '!');
        assert(tam_sl_idx(s1, -2) == 'd');
        assert(tam_sl_eqstr(s1, "Hello, world!"));

        tam_slice_t hello = tam_slice_prefix(s1, 5);
        assert(tam_sl_idx(hello, 0) == 'H');
        assert(tam_sl_idx(hello, 4) == 'o');
        assert(tam_sl_idx(hello, -1) == 'o');
        assert(tam_sl_idx(hello, -2) == 'l');
        assert(hello.len == 5);
        assert(tam_sl_eqstr(hello, "Hello"));

        tam_slice_t world = tam_slice_suffix(s1, 7);
        assert(tam_sl_idx(world, 0) == 'w');
        assert(tam_sl_idx(world, -1) == '!');
        assert(world.len == 6);
        assert(tam_sl_eqstr(world, "world!"));

        tam_slice_t llo = tam_reslice(s1, 2, 5);
        assert(tam_sl_idx(llo, 0) == 'l');
        assert(tam_sl_idx(llo, -1) == 'o');
        assert(llo.len == 3);

        tam_slice_t llo2 = tam_slice_suffix(hello, 2);
        assert(llo.len == llo2.len);
        assert(llo.buf == llo2.buf);
        assert(tam_sl_eqv(llo2, llo));
        assert(tam_sl_eq(llo2, llo));
        
        // equality
        tam_slice_t llo3 = tam_slice("llo");
        assert(!tam_sl_eqv(llo, llo3));
        assert(tam_sl_eq(llo, llo3));
        assert(!tam_sl_eq(llo3, hello));
        assert(!tam_sl_eq(llo3, tam_slice("ll")));
        assert(!tam_sl_eq(llo3, tam_slice("llo3")));

        // finding chars and tokenizing
        assert(tam_sl_cspan(s1, ",") == 5);
        assert(tam_sl_cspan(s1, "0") == s1.len);
        assert(tam_sl_cspan(s1, " ") == 6);
        assert(tam_sl_cspan(hello, "w") == hello.len);

        assert(tam_sl_startswithstr(s1, "Hel"));
        assert(tam_sl_startswithstr(s1, "Hello"));
        assert(!tam_sl_startswithstr(s1, "Hello, world!!!!"));
        assert(!tam_sl_startswithstr(s1, "hello"));

        assert(tam_sl_startswith(s1, slice("Hel")));
    }
    {
        // stripping whitespace
        tam_slice_t sl = slice("    a string with spaces\t ");
        tam_slice_t sl2 = sl, sl3 = sl, sl4 = sl;
        assert(tam_sl_eqv(sl, sl2) && tam_sl_eq(sl, sl2));
        int64_t leading = tam_sl_lstrip(&sl2);
        assert(leading == 4);
        assert(tam_sl_eq(sl2, tam_slice_suffix(sl, leading)));
        assert(tam_sl_eq(sl2, tam_slice_lstrip(sl)));
        assert(tam_sl_eq(sl2, tam_reslice(sl, leading, sl.len)));
        assert(tam_sl_eq(sl2, tam_slice_lstrip(sl2)));

        int64_t trailing = tam_sl_rstrip(&sl3);
        assert(trailing == 24);
        assert(tam_sl_eq(sl3, tam_slice_prefix(sl, trailing)));
        assert(tam_sl_eq(sl3, tam_slice_rstrip(sl)));
        assert(tam_sl_eq(sl3, tam_reslice(sl, 0, trailing)));
        assert(tam_sl_eq(sl3, tam_slice_rstrip(sl3)));

        int64_t stripped = sl_strip(&sl4);
        assert(stripped == 6);
        assert(tam_sl_eq(sl4, tam_reslice(sl, leading, trailing)));
        assert(tam_sl_eq(sl4, tam_slice_strip(sl)));
        assert(tam_sl_eq(sl4, tam_slice_strip(sl4)));
        assert(tam_sl_eq(sl4, tam_slice_lstrip(sl3)));
        assert(tam_sl_eq(sl4, tam_slice_rstrip(sl2)));
    }
    {
        // Tokenizing
        const char *sentence = "a few words to check, with punctuation.";
        tam_slice_t words = tam_slice(sentence);
        const char *dlm = ",. ";
        assert(tam_sl_eqstr(tam_slice_tok(&words, dlm), "a"));
        assert(tam_sl_eqstr(tam_slice_tok(&words, dlm), "few"));
        assert(tam_sl_eqstr(tam_slice_tok(&words, dlm), "words"));
        assert(tam_sl_eqstr(tam_slice_tok(&words, dlm), "to"));
        assert(tam_sl_eqstr(tam_slice_tok(&words, dlm), "check"));
        assert(tam_sl_eqstr(tam_slice_tok(&words, dlm), "with"));
        assert(tam_sl_eqstr(tam_slice_tok(&words, dlm), "punctuation"));
        assert(tam_sl_eqstr(tam_slice_tok(&words, dlm), ""));
        assert(words.len == 0);
        assert(words.buf == sentence + strlen(sentence));
        assert(*words.buf == '\0');

        const char *paragraph = "Here's a sentence.\n"
                                "Here's another.\r\n"
                                "And here's one more!\r\n";
        tam_slice_t par = slice(paragraph);
        assert(tam_sl_eqstr(slice_getline(&par), "Here's a sentence."));
        assert(tam_sl_eqstr(slice_getline(&par), "Here's another."));
        assert(tam_sl_eqstr(slice_getline(&par), "And here's one more!"));
        assert(tam_sl_eqstr(slice_getline(&par), ""));

    }
    {
        // finding
        const char *str = "word1 word2 word3 word4 wor5 word6";
        tam_slice_t sl = tam_slice(str);
        assert(tam_sl_findstr(sl, "word") == 0);
        assert(tam_sl_findstr(sl, "word1") == 0);
        assert(tam_sl_findstr(sl, "word2") == 6);
        assert(tam_sl_findstr(sl, "word3") == 12);
        assert(tam_sl_findstr(sl, "word4") == 18);
        assert(tam_sl_findstr(sl, "wor5") == 24);
        assert(tam_sl_findstr(sl, "word5") == sl.len);
        assert(tam_sl_findstr(sl, "word6") == 29);
        assert(tam_sl_findstr(sl, "\0") == 0);
        assert(tam_sl_findstr(sl, "") == 0);
    }
    return 0;
}
// end slice tests }}}

// ### StringBuilder tests {{{
int tam_test_stringbuilders () {
    // basic allocation, construction, and deallocation
    tam_stringbuilder_t sb = tam_sb_new();
    assert(sb.buf == NULL);
    assert(sb.len == 0);
    assert(sb.cap == 0);

    tam_sb_appendcharsn(&sb, "Hello", 5);
    assert(sb.buf != NULL);
    assert(sb.cap == TAM_SB_INITIAL_CAPACITY);
    assert(sb.len == 5);

    tam_sb_appendf(&sb, "%s", ", ");
    assert(sb.cap == TAM_SB_INITIAL_CAPACITY);
    assert(sb.len == 7);

    tam_sb_appendchars(&sb, "world!");
    assert(sb.cap == TAM_SB_INITIAL_CAPACITY);
    assert(sb.len == 13);

    tam_slice_t next = slice(" Here's another sentence that should cause the buffer to reallocate.");
    tam_sb_appendslice(&sb, next);
    assert((int64_t)sb.len == 13 + next.len);
    assert((int64_t)sb.cap == 13 + next.len+1);

    char *sentence = tam_sb_tochars(sb);
    const char *expected = "Hello, world! Here's another sentence that should cause the buffer to reallocate.";
    assert(strcmp(sentence, expected) == 0);
    tam_deallocate(sentence);

    tam_sb_deallocate(&sb);
    assert(sb.buf == NULL);
    assert(sb.len == 0);
    assert(sb.cap == 0);
    return 0;
}

// end StringBuilder tests }}}

int tam_test_strings() {
    int failure = 0;
    failure += tam_test_slices();
    failure += tam_test_stringbuilders(); 
    if (failure == 0) {
        printf("\x1b[1;32m" "Tests passed!" "\x1b[0m" "\n");
        return 0;
    } else {
        printf("\x1b[1;31m" "%d tests failed!" "\x1b[0m" "\n", failure);
        return 1;
    }
    return failure;
}
#endif // TAM_STRINGS_TEST

#endif // TAM_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // TAM_STRINGS_H
