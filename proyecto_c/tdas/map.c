#include "map.h"
#include <stdint.h>
#include <stdlib.h>

#define CAPACIDAD_INICIAL 16   /* siempre potencia de 2 */

typedef struct MapNode {
  MapPair pair;           /* primer campo: &nodo->pair apunta al propio nodo */
  size_t hash;            /* hash ya mezclado: acelera comparaciones y evita recalcular al crecer */
  struct MapNode *next;   /* siguiente nodo de la misma cubeta */
} MapNode;

struct Map {
  size_t (*hash)(void *key);
  int (*is_equal)(void *key1, void *key2);
  MapNode **buckets;      /* arreglo de cubetas; cada una es una cadena de nodos */
  size_t capacity;        /* M: número de cubetas */
  size_t size;            /* N: parejas almacenadas */
  size_t cursor_bucket;   /* estado de map_first / map_next */
  MapNode *cursor_node;
};

/* Mezcla final de bits (finalizador de MurmurHash3). Así el índice de cubeta
   (hash & (M-1)) queda bien repartido aunque la función hash del usuario sea simple. */
static size_t mezclar(size_t h) {
  uint64_t x = (uint64_t)h;
  x ^= x >> 33;
  x *= 0xff51afd7ed558ccdULL;
  x ^= x >> 33;
  x *= 0xc4ceb9fe1a85ec53ULL;
  x ^= x >> 33;
  return (size_t)x;
}

Map *map_create(size_t (*hash)(void *key), int (*is_equal)(void *key1, void *key2)) {
  if (hash == NULL || is_equal == NULL) return NULL;

  Map *map = (Map *)malloc(sizeof(Map));
  if (map == NULL) return NULL;

  map->buckets = (MapNode **)calloc(CAPACIDAD_INICIAL, sizeof(MapNode *));
  if (map->buckets == NULL) {
    free(map);
    return NULL;
  }
  map->hash = hash;
  map->is_equal = is_equal;
  map->capacity = CAPACIDAD_INICIAL;
  map->size = 0;
  map->cursor_bucket = 0;
  map->cursor_node = NULL;
  return map;
}

/* Recorre la cadena de la cubeta que corresponde a la clave. */
static MapNode *buscar_nodo(Map *map, void *key, size_t h) {
  MapNode *n = map->buckets[h & (map->capacity - 1)];
  while (n != NULL) {
    if (n->hash == h && map->is_equal(n->pair.key, key)) return n;
    n = n->next;
  }
  return NULL;
}

/* Duplica las cubetas y reubica los nodos existentes (sin reservar nodos nuevos).
   Cuesta O(N), pero ocurre solo cuando N pasa a M, 2M, 4M...: O(1) amortizado por inserción. */
static void crecer(Map *map) {
  size_t nueva_cap = map->capacity * 2;
  MapNode **nuevas = (MapNode **)calloc(nueva_cap, sizeof(MapNode *));
  if (nuevas == NULL) return;   /* sin memoria: se sigue con la tabla actual (cadenas más largas) */

  for (size_t i = 0; i < map->capacity; i++) {
    MapNode *n = map->buckets[i];
    while (n != NULL) {
      MapNode *sig = n->next;
      size_t j = n->hash & (nueva_cap - 1);
      n->next = nuevas[j];
      nuevas[j] = n;
      n = sig;
    }
  }
  free(map->buckets);
  map->buckets = nuevas;
  map->capacity = nueva_cap;
  map->cursor_bucket = 0;       /* un recorrido en curso queda invalidado */
  map->cursor_node = NULL;
}

void multimap_insert(Map *map, void *key, void *value) {
  MapNode *n = (MapNode *)malloc(sizeof(MapNode));
  if (n == NULL) return;

  n->pair.key = key;
  n->pair.value = value;
  n->hash = mezclar(map->hash(key));

  size_t i = n->hash & (map->capacity - 1);
  n->next = map->buckets[i];    /* inserción al inicio de la cadena: O(1) */
  map->buckets[i] = n;
  map->size++;

  if (map->size > map->capacity) crecer(map);   /* mantiene el factor de carga <= 1 */
}

void map_insert(Map *map, void *key, void *value) {
  if (map_search(map, key) != NULL) return;
  multimap_insert(map, key, value);
}

MapPair *map_search(Map *map, void *key) {
  MapNode *n = buscar_nodo(map, key, mezclar(map->hash(key)));
  return n ? &n->pair : NULL;
}

MapPair *map_remove(Map *map, void *key) {
  size_t h = mezclar(map->hash(key));
  MapNode **enlace = &map->buckets[h & (map->capacity - 1)];

  while (*enlace != NULL) {
    MapNode *n = *enlace;
    if (n->hash == h && map->is_equal(n->pair.key, key)) {
      MapPair *resultado = (MapPair *)malloc(sizeof(MapPair));
      if (resultado == NULL) return NULL;
      *resultado = n->pair;
      *enlace = n->next;
      if (map->cursor_node == n) map->cursor_node = NULL;
      free(n);
      map->size--;
      return resultado;
    }
    enlace = &n->next;
  }
  return NULL;
}

/* Desde la cubeta "desde" en adelante, deja el cursor en el primer nodo que encuentre. */
static MapPair *avanzar_cursor(Map *map, size_t desde) {
  for (size_t i = desde; i < map->capacity; i++) {
    if (map->buckets[i] != NULL) {
      map->cursor_bucket = i;
      map->cursor_node = map->buckets[i];
      return &map->cursor_node->pair;
    }
  }
  map->cursor_node = NULL;
  return NULL;
}

MapPair *map_first(Map *map) { return avanzar_cursor(map, 0); }

MapPair *map_next(Map *map) {
  if (map->cursor_node == NULL) return NULL;
  if (map->cursor_node->next != NULL) {
    map->cursor_node = map->cursor_node->next;
    return &map->cursor_node->pair;
  }
  return avanzar_cursor(map, map->cursor_bucket + 1);
}

size_t map_size(Map *map) { return map->size; }

void map_clean(Map *map) {
  for (size_t i = 0; i < map->capacity; i++) {
    MapNode *n = map->buckets[i];
    while (n != NULL) {
      MapNode *sig = n->next;
      free(n);
      n = sig;
    }
    map->buckets[i] = NULL;
  }
  map->size = 0;
  map->cursor_bucket = 0;
  map->cursor_node = NULL;
}

void map_destroy(Map *map) {
  if (map == NULL) return;
  map_clean(map);
  free(map->buckets);
  free(map);
}
