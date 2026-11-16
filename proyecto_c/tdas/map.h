#ifndef MAP_H
#define MAP_H

#include "list.h"

typedef struct {
    void *key;
    void *value;
} MapPair;

typedef struct Map Map;
typedef unsigned long (*MapHash)(void *key);

Map *map_create(int (*is_equal)(void *, void *));
Map *map_create_hash(int (*is_equal)(void *, void *), MapHash hash);
Map *sorted_map_create(int (*lower_than)(void *, void *));

void map_insert(Map *map, void *key, void *value);
MapPair *map_remove(Map *map, void *key);
MapPair *map_search(Map *map, void *key);

MapPair *map_first(Map *map);
MapPair *map_next(Map *map);

void map_clean(Map *map);
void map_destroy(Map *map);

#endif