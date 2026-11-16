#include "map.h"
#include <stdlib.h>

typedef struct MapNode {
    MapPair *pair;
    struct MapNode *next;
    struct MapNode *bucket_next;
} MapNode;

struct Map {
    int (*lower_than)(void *, void *);
    int (*is_equal)(void *, void *);
    MapHash hash;
    MapNode **buckets;
    size_t bucket_count;
    MapNode *head;
    MapNode *tail;
    MapNode *current;
    size_t size;
};

static Map *create_map(int (*eq)(void *, void *), int (*lt)(void *, void *),
                       MapHash hash) {
    Map *m = calloc(1, sizeof(Map));
    if (!m) return NULL;
    m->is_equal = eq;
    m->lower_than = lt;
    m->hash = hash;
    if (hash) {
        m->bucket_count = 1024;
        m->buckets = calloc(m->bucket_count, sizeof(MapNode *));
        if (!m->buckets) { free(m); return NULL; }
    }
    return m;
}

Map *map_create(int (*is_equal)(void *, void *)) {
    return is_equal ? create_map(is_equal, NULL, NULL) : NULL;
}

Map *map_create_hash(int (*is_equal)(void *, void *), MapHash hash) {
    return (is_equal && hash) ? create_map(is_equal, NULL, hash) : NULL;
}

Map *sorted_map_create(int (*lower_than)(void *, void *)) {
    return lower_than ? create_map(NULL, lower_than, NULL) : NULL;
}

static int keys_equal(Map *map, void *a, void *b) {
    if (map->is_equal) return map->is_equal(a, b);
    if (map->lower_than)
        return !map->lower_than(a, b) && !map->lower_than(b, a);
    return a == b;
}

static int resize_hash(Map *map) {
    size_t nuevo = map->bucket_count * 2;
    MapNode **b = calloc(nuevo, sizeof(MapNode *));
    if (!b) return 0;
    for (MapNode *n = map->head; n; n = n->next) {
        size_t i = map->hash(n->pair->key) % nuevo;
        n->bucket_next = b[i];
        b[i] = n;
    }
    free(map->buckets);
    map->buckets = b;
    map->bucket_count = nuevo;
    return 1;
}

MapPair *map_search(Map *map, void *key) {
    if (!map || !key) return NULL;
    if (map->hash) {
        size_t i = map->hash(key) % map->bucket_count;
        for (MapNode *n = map->buckets[i]; n; n = n->bucket_next) {
            if (map->is_equal(n->pair->key, key)) {
                map->current = n;
                return n->pair;
            }
        }
        return NULL;
    }
    for (MapNode *n = map->head; n; n = n->next) {
        if (keys_equal(map, n->pair->key, key)) {
            map->current = n;
            return n->pair;
        }
    }
    return NULL;
}

void map_insert(Map *map, void *key, void *value) {
    if (!map || !key || map_search(map, key)) return;

    MapPair *pair = malloc(sizeof(MapPair));
    MapNode *node = calloc(1, sizeof(MapNode));
    if (!pair || !node) { free(pair); free(node); return; }
    pair->key = key;
    pair->value = value;
    node->pair = pair;

    if (map->lower_than) {
        MapNode *prev = NULL, *cur = map->head;
        while (cur && !map->lower_than(key, cur->pair->key)) {
            prev = cur;
            cur = cur->next;
        }
        if (!prev) {
            node->next = map->head;
            map->head = node;
        } else {
            node->next = prev->next;
            prev->next = node;
        }
        if (!node->next) map->tail = node;
        if (!map->tail) map->tail = node;
    } else {
        if (map->tail) map->tail->next = node;
        else map->head = node;
        map->tail = node;
    }

    if (map->hash) {
        size_t i = map->hash(key) % map->bucket_count;
        node->bucket_next = map->buckets[i];
        map->buckets[i] = node;
    }
    map->size++;
    if (map->hash && map->size > map->bucket_count)
        (void)resize_hash(map);
}

MapPair *map_remove(Map *map, void *key) {
    if (!map || !key) return NULL;
    MapNode *prev = NULL, *cur = map->head;
    while (cur && !keys_equal(map, cur->pair->key, key)) {
        prev = cur;
        cur = cur->next;
    }
    if (!cur) return NULL;

    if (prev) prev->next = cur->next;
    else map->head = cur->next;
    if (map->tail == cur) map->tail = prev;

    if (map->hash) {
        size_t i = map->hash(key) % map->bucket_count;
        MapNode *bp = NULL, *bn = map->buckets[i];
        while (bn && bn != cur) { bp = bn; bn = bn->bucket_next; }
        if (bn) {
            if (bp) bp->bucket_next = bn->bucket_next;
            else map->buckets[i] = bn->bucket_next;
        }
    }
    if (map->current == cur) map->current = NULL;
    MapPair *pair = cur->pair;
    free(cur);
    map->size--;
    return pair;
}

MapPair *map_first(Map *map) {
    if (!map) return NULL;
    map->current = map->head;
    return map->current ? map->current->pair : NULL;
}

MapPair *map_next(Map *map) {
    if (!map || !map->current) return NULL;
    map->current = map->current->next;
    return map->current ? map->current->pair : NULL;
}

void map_clean(Map *map) {
    if (!map) return;
    MapNode *n = map->head;
    while (n) {
        MapNode *next = n->next;
        free(n->pair);
        free(n);
        n = next;
    }
    map->head = map->tail = map->current = NULL;
    map->size = 0;
    if (map->buckets)
        for (size_t i = 0; i < map->bucket_count; i++) map->buckets[i] = NULL;
}

void map_destroy(Map *map) {
    if (!map) return;
    map_clean(map);
    free(map->buckets);
    free(map);
}
