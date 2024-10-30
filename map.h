
#ifndef TAM_MAP_H
#define TAM_MAP_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <tam/string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* tam_any;

typedef struct tam_pair {
    char *key;
    tam_any *value;
} tam_pair;

typedef struct tam_map {
    size_t count;
    size_t capacity;
    tam_pair *entries;
} tam_map;


void tam_init_map(tam_map *map);
void tam_free_map(tam_map *map);

bool tam_map_set(tam_map *map, const tam_str key, tam_any val);
bool tam_map_get(tam_map *map, const tam_str key, tam_any *val);

/*** Implementation ***/
#ifdef TAM_MAP_IMPLEMENTATION

/*** includes ***/
#include <tam/memory.h>

/*** defines ***/
#define MAP_MAX_LOAD 0.75

/*** prototypes ***/
tam_pair *find_entry(tam_pair *entries, int capacity, tam_str key);

/*** memory management ***/
void tam_init_map(tam_map *map) {
    map->count = 0;
    map->capacity = 0;
    map->entries = NULL;
}

void tam_free_map(tam_map *map) {
    if (map->entries != NULL) {
        tam_free(map->entries);
    }
    tam_init_map(map);
}

static void adjust_capacity(tam_map *map, int capacity) {
    tam_pair *entries = tam_alloc(tam_pair, capacity);
    for (int i = 0; i < capacity; i++) {
        entries[i].key = NULL;
        entries[i].value = NULL;
    }

    // reinsert entries into new list
    for (int i = 0; i < map->count; i++) {
        tam_pair *entry = &map->entries[i];
        if (entry->key == NULL) continue;

        tam_pair *dest = find_entry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
    }

    // free old entries, and replace with new ones
    tam_free(map->entries);
    map->entries = entries;
    map->capacity = capacity;
}

/*** hashing ***/
static uint32_t hash_string(const tam_str(key)) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < tam_strlen(key); i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

/*** entry retrieval ***/
tam_pair* find_entry(tam_pair *entries, int capacity, const tam_str key) {
    uint32_t hash = hash_string(key) % capacity;
    uint32_t index = hash;
    for (;;) {
        tam_pair *entry = &entries[index];
        if (entry->key == NULL) {
            // no entry at this index
            return entry;
        }
        // check to see if the key at this index matches our key
        // first, compare hashes, then compare individual chars
        uint32_t entry_hash = hash_string(entry->key) % capacity;
        if (hash == entry_hash &&
            memcmp(key, entry->key, tam_strlen(key)) == 0) {
            // found the key
            return entry;
        }
        // collision: begin probing
        index = (index + 1) % capacity;
    }
}

#define GROW_CAPACITY(x) (x == 0 ? 8 : x * 2);

bool tam_map_set(tam_map *map, tam_str key, tam_any val) {
    // allocate or extend entries array
    if (map->count + 1 > map->capacity * MAP_MAX_LOAD) {
        int new_capacity = GROW_CAPACITY(map->capacity);
        adjust_capacity(map, new_capacity);
    }
    tam_pair *entry = find_entry(map->entries, map->capacity, key);
    bool is_new_key = entry->key == NULL;
    if (is_new_key) map->count++;

    entry->key = key;
    entry->value = val;
    return is_new_key;
}

bool tam_map_get(tam_map *map, tam_str key, tam_any *val) {
    if (map->count == 0) return false;

    tam_pair *entry = find_entry(map->entries, map->capacity, key);
    if (entry->key == NULL) return false;

    *val = entry->value;
    return true;
}

#endif // TAM_MAP_IMPLEMENTATION

/*** tests ***/
#ifdef TAM_MAP_TEST

/*** includes ***/
#include <assert.h>
#include <tam/colors.h>

/*** body ***/

void tam_map_test() {
    #define str tam_strnew
    
    printf(BBLK "tam_map: " CRESET);

    {
        tam_map map;
        tam_init_map(&map);
        assert(map.count == 0);
        assert(map.capacity == 0);
        assert(map.entries == NULL);

        tam_str key = str("key_1"); 
        tam_str val_1 = str("value_1");
        assert(!tam_map_get(&map, key, &val_1));

        tam_map_set(&map, key, val_1);
        tam_str result;
        assert(tam_map_get(&map, key, &result));
        assert(map.capacity == 8);
        assert(map.count == 1);
        assert(result == val_1);

        tam_str val_2 = str("value_2");
        tam_map_set(&map, key, val_2);
        assert(tam_map_get(&map, key, &result));
        assert(map.capacity == 8);
        assert(map.count == 1);
        assert(result == val_2);
        assert(result != val_1);

        tam_str val_3 = str("value_3");
        tam_str val_4 = str("value_4");
        tam_str val_5 = str("value_5");
        tam_str val_6 = str("value_6");
        tam_str val_7 = str("value_7");
        tam_str val_8 = str("value_8");
        tam_str val_9 = str("value_9");

        tam_map_set(&map, str("key_1"), val_1);
        tam_map_set(&map, str("key_2"), val_2);
        tam_map_set(&map, str("key_3"), val_3);
        tam_map_set(&map, str("key_4"), val_4);
        tam_map_set(&map, str("key_5"), val_5);
        tam_map_set(&map, str("key_6"), val_6);
        tam_map_set(&map, str("key_7"), val_7);
        tam_map_set(&map, str("key_8"), val_8);
        tam_map_set(&map, str("key_9"), val_9);

        assert(map.capacity == 16);
        assert(map.count == 9);

        tam_map_set(&map, str("key_1"), val_9);
        assert(map.capacity == 16);
        assert(map.count == 9);

        assert(tam_map_get(&map, str("key_1"), &result));
        assert(result == val_9);
    }

    printf(GRN "All tests passed" CRESET "\n");
    #undef str
}

#endif // TAM_MAP_TEST
 
#ifdef __cplusplus
}
#endif // __cplusplus
#endif // TAM_MAP_H
