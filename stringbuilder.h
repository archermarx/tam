#ifndef TAM_STRINGBUILDER_H
#define TAM_STRINGBUILDER_H

#ifdef __cplusplus
extern "C" {
#endif

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
    int len;
    int cap;
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
void tam_sb_appendchars(tam_stringbuilder_t *sb, const char *s);

/*
 * Append a char array with known length to a StringBuilder
 */
void tam_sb_appendcharsn(tam_stringbuilder_t *sb, const char *s, usize n);

/*
 * Append a formatted string to StringBuilder
 */
void tam_sb_appendf(tam_stringbuilder_t *sb, const char *fmt, ...);

/*
 * Construct a char array from a Stringbuilder
 */
char *tam_sb_tochars(tam_stringbuilder_t sb);

#if defined(USING_NAMESPACE_TAM) || defined(USING_TAM_STRINGS) ///{{{
typedef tam_stringbuilder_t StringBuilder;
#define sb_new tam_sb_new
#define sb_deallocate tam_sb_deallocate
#define sb_appendslice tam_sb_appendslice
#define sb_appendchars tam_sb_appendchars
#define sb_appendcharsn tam_sb_appendcharsn
#define sb_appendf tam_sb_appendf
#define sb_tochars tam_sb_tochars
#endif // end StringBuilder namespace }}}

// end StringBuilder declarations }}}
//
// ### StringBuilder implementation {{{

#define TAM_SB_INITIAL_CAPACITY 16

#if defined(TAM_IMPLEMENTATION)

tam_stringbuilder_t tam_sb_new() { return (tam_stringbuilder_t){.cap = 0, .len = 0, .buf = NULL}; }

void tam_sb_deallocate(tam_stringbuilder_t *sb) {
    free(sb->buf);
    sb->cap = 0;
    sb->len = 0;
}

static void tam_sb_grow(tam_stringbuilder_t *sb, usize newlen) {
    if (newlen <= sb->cap)
        return;
    usize newcap = sb->buf == NULL ? TAM_SB_INITIAL_CAPACITY : 2 * sb->cap;
    if (newcap < newlen)
        newcap = newlen + 1;
    sb->buf = tam_reallocate(sb->buf, char, newcap);
    sb->cap = newcap;
}

// Appending
void tam_sb_appendcharsn(tam_stringbuilder_t *sb, const char *s, usize n) {
    usize newlen = sb->len + n;
    tam_sb_grow(sb, newlen);
    memcpy(sb->buf + sb->len, s, n);
    sb->len = newlen;
}

void tam_sb_appendslice(tam_stringbuilder_t *sb, tam_slice_t sl) { tam_sb_appendcharsn(sb, sl.buf, sl.len); }

void tam_sb_appendchars(tam_stringbuilder_t *sb, const char *s) { tam_sb_appendcharsn(sb, s, strlen(s)); }

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

#if defined(TAM_TEST)

// ### StringBuilder tests {{{
int tam_test_stringbuilders() {
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
    assert((int)sb.len == 13 + next.len);
    assert((int)sb.cap == 13 + next.len + 1);

    char *sentence = tam_sb_tochars(sb);
    const char *expected = "Hello, world! Here's another sentence that should cause the buffer to reallocate.";
    assert(strcmp(sentence, expected) == 0);
    tam_deallocate(sentence);

    tam_sb_deallocate(&sb);
    assert(sb.buf == NULL);
    assert(sb.len == 0);
    assert(sb.cap == 0);

    printf("\x1b[1;32m" "Tests passed!" "\x1b[0m\n");
    return 0;
}

// end StringBuilder tests }}}
#endif // TAM_TEST

#endif // TAM_IMPLEMENTATION
//
#ifdef __cplusplus
}
#endif

#endif // TAM_STRINGBUILDER_H
