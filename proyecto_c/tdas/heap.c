
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "heap.h"

typedef struct {
    void* data;
    unsigned long long order;
} HeapElem;

struct Heap {
    HeapElem* heapArray;
    size_t size;
    size_t capacity;
    unsigned long long next_order;

    int (*compare)(void*, void*);
};

/* Compara dos elementos considerando su prioridad y orden de inserción. */
static int higher_priority(Heap* pq, HeapElem a, HeapElem b) {

    int result = pq->compare(a.data, b.data);

    if (result != 0) {
        return result > 0;
    }

    return a.order < b.order;
}

/* Intercambia dos elementos. */
static void swap(HeapElem* a, HeapElem* b) {

    HeapElem temp = *a;
    *a = *b;
    *b = temp;
}

/* Aumenta la capacidad del arreglo dinámico. */
static int resize_heap(Heap* pq) {

    size_t new_capacity;

    if (pq->capacity == 0) {
        new_capacity = 8;
    } else {

        if (pq->capacity > SIZE_MAX / 2) {
            return 0;
        }

        new_capacity = pq->capacity * 2;
    }

    if (new_capacity > SIZE_MAX / sizeof(HeapElem)) {
        return 0;
    }

    HeapElem* temp = realloc(
        pq->heapArray,
        new_capacity * sizeof(HeapElem)
    );

    if (temp == NULL) {
        return 0;
    }

    pq->heapArray = temp;
    pq->capacity = new_capacity;

    return 1;
}

/* Flotación: mueve un elemento hacia la raíz. */
static void sift_up(Heap* pq, size_t index) {

    while (index > 0) {

        size_t parent = (index - 1) / 2;

        if (!higher_priority(
                pq,
                pq->heapArray[index],
                pq->heapArray[parent])) {
            break;
        }

        swap(
            &pq->heapArray[index],
            &pq->heapArray[parent]
        );

        index = parent;
    }
}

/* Hundimiento: mueve un elemento hacia las hojas. */
static void sift_down(Heap* pq, size_t index) {

    while (1) {

        size_t left = 2 * index + 1;
        size_t right = 2 * index + 2;
        size_t best = index;

        if (left < pq->size &&
            higher_priority(
                pq,
                pq->heapArray[left],
                pq->heapArray[best])) {

            best = left;
        }

        if (right < pq->size &&
            higher_priority(
                pq,
                pq->heapArray[right],
                pq->heapArray[best])) {

            best = right;
        }

        if (best == index) {
            break;
        }

        swap(
            &pq->heapArray[index],
            &pq->heapArray[best]
        );

        index = best;
    }
}

/* Crea un Heap vacío. */
Heap* heap_create(int (*compare)(void*, void*)) {

    if (compare == NULL) {
        return NULL;
    }

    Heap* pq = malloc(sizeof(Heap));

    if (pq == NULL) {
        return NULL;
    }

    pq->heapArray = NULL;
    pq->size = 0;
    pq->capacity = 0;
    pq->next_order = 0;
    pq->compare = compare;

    return pq;
}

/* Inserta un elemento. Complejidad: O(log n). */
int heap_push(Heap* pq, void* data) {

    if (pq == NULL || data == NULL) {
        return 0;
    }

    if (pq->next_order == ULLONG_MAX) {
        return 0;
    }

    if (pq->size == pq->capacity) {

        if (!resize_heap(pq)) {
            return 0;
        }
    }

    size_t index = pq->size;

    pq->heapArray[index].data = data;
    pq->heapArray[index].order = pq->next_order;

    pq->size++;
    pq->next_order++;

    sift_up(pq, index);

    return 1;
}

/*
 * Construye el Heap desde una lista utilizando Floyd.
 * Complejidad: O(n).
 * No libera ni modifica los datos de la lista.
 */
int heap_build(Heap* pq, List* lista) {

    if (pq == NULL || lista == NULL) {
        return 0;
    }

    int count = list_size(lista);

    if (count < 0) {
        return 0;
    }

    size_t n = (size_t) count;

    if (n > SIZE_MAX / sizeof(HeapElem)) {
        return 0;
    }

    HeapElem* new_array = NULL;

    if (n > 0) {

        new_array = malloc(n * sizeof(HeapElem));

        if (new_array == NULL) {
            return 0;
        }
    }

    void* data = list_first(lista);

    for (size_t i = 0; i < n; i++) {

        if (data == NULL) {
            free(new_array);
            return 0;
        }

        new_array[i].data = data;
        new_array[i].order = i;

        if (i + 1 < n) {
            data = list_next(lista);
        }
    }

    free(pq->heapArray);

    pq->heapArray = new_array;
    pq->size = n;
    pq->capacity = n;
    pq->next_order = n;

    /* Construcción de abajo hacia arriba mediante Floyd. */
    if (pq->size > 1) {

        for (size_t i = pq->size / 2; i > 0; i--) {
            sift_down(pq, i - 1);
        }
    }

    return 1;
}

/* Consulta el elemento de mayor prioridad. Complejidad: O(1). */
void* heap_top(Heap* pq) {

    if (pq == NULL || pq->size == 0) {
        return NULL;
    }

    return pq->heapArray[0].data;
}

/* Extrae el elemento de mayor prioridad. Complejidad: O(log n). */
void* heap_pop(Heap* pq) {

    if (pq == NULL || pq->size == 0) {
        return NULL;
    }

    void* result = pq->heapArray[0].data;

    pq->size--;

    if (pq->size > 0) {

        pq->heapArray[0] = pq->heapArray[pq->size];

        sift_down(pq, 0);
    }

    return result;
}

/* Comprueba si el Heap está vacío. */
int heap_empty(Heap* pq) {

    return pq == NULL || pq->size == 0;
}

/* Libera el Heap sin liberar los datos almacenados. */
void heap_destroy(Heap* pq) {

    if (pq == NULL) {
        return;
    }

    free(pq->heapArray);
    free(pq);
}
