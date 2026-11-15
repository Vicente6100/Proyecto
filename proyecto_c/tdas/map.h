#ifndef MAP_H
#define MAP_H
#include "list.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  void *key;
  void *value;
} MapPair;

typedef struct Map Map;

/*
 * Map implementado como TABLA HASH CON ENCADENAMIENTO.
 *   hash:     función que convierte una clave en un número (ver hash_str / hash_int en main.c)
 *   is_equal: devuelve 1 si las dos claves son iguales
 * Búsqueda, inserción y eliminación: O(1) esperado (O(N) solo si todas las claves
 * colisionan). El factor de carga se mantiene <= 1 duplicando las cubetas cuando hace falta.
 */
Map *map_create(size_t (*hash)(void *key), int (*is_equal)(void *key1, void *key2));

/* Inserta solo si la clave no existe todavía. */
void map_insert(Map *map, void *key, void *value);

/* Inserta aunque la clave ya exista (multimapa). map_search devuelve la más reciente. */
void multimap_insert(Map *map, void *key, void *value);

/* Quita la clave y devuelve una pareja reservada con malloc (quien llama la libera con free),
   o NULL si no existía. No libera la clave ni el valor. */
MapPair *map_remove(Map *map, void *key);

MapPair *map_search(Map *map, void *key);

/* Recorrido de todas las parejas (en orden arbitrario). No insertar ni quitar mientras se recorre. */
MapPair *map_first(Map *map);

MapPair *map_next(Map *map);

/* Cantidad de parejas almacenadas, en O(1). */
size_t map_size(Map *map);

/* Quita todas las parejas (no libera claves ni valores). */
void map_clean(Map *map);

/* Libera el Map completo (no libera claves ni valores). */
void map_destroy(Map *map);

#endif /* MAP_H */
