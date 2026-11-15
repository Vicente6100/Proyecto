
#ifndef HEAP_H
#define HEAP_H

#include "list.h"

typedef struct Heap Heap;

/*
 * Crea un Max-Heap.
 * compare devuelve:
 *  1 si el primer elemento tiene mayor prioridad.
 * -1 si el segundo elemento tiene mayor prioridad.
 *  0 si tienen la misma prioridad.
 */
Heap* heap_create(int (*compare)(void*, void*));

/* Inserta un elemento en el Heap. */
int heap_push(Heap* pq, void* data);

/* Construye el Heap desde una lista utilizando Floyd. */
int heap_build(Heap* pq, List* lista);

/* Consulta el elemento de mayor prioridad. */
void* heap_top(Heap* pq);

/* Extrae y devuelve el elemento de mayor prioridad. */
void* heap_pop(Heap* pq);

/* Comprueba si el Heap está vacío. */
int heap_empty(Heap* pq);

/* Libera la memoria interna del Heap. */
void heap_destroy(Heap* pq);

#endif
