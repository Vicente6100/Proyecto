
#include <stdio.h>
#include "tdas/heap.h"
#include "tdas/pelicula.h"
#include "tdas/list.h"
#include <stdlib.h>

int compararPeliculas(void* a, void* b) {

    Pelicula* p1 = a;
    Pelicula* p2 = b;

    if (p1->rating > p2->rating) return 1;
    if (p1->rating < p2->rating) return -1;

    if (p1->times_seen > p2->times_seen) return 1;
    if (p1->times_seen < p2->times_seen) return -1;

    return 0;
}

int main() {

    Pelicula p1 = {.id = 1, .rating = 8, .times_seen = 2};
    Pelicula p2 = {.id = 2, .rating = 10, .times_seen = 1};
    Pelicula p3 = {.id = 3, .rating = 10, .times_seen = 3};
    Pelicula p4 = {.id = 4, .rating = 9, .times_seen = 5};

    List* lista = list_create();

    list_pushBack(lista, &p1);
    list_pushBack(lista, &p2);
    list_pushBack(lista, &p3);
    list_pushBack(lista, &p4);

    Heap* ranking = heap_create(compararPeliculas);

    if (!heap_build(ranking, lista)) {
        printf("Error al construir el Heap.\n");
        heap_destroy(ranking);
        list_destroy(lista);
        return 1;
    }

    printf("=== RANKING CON HEAP BUILD ===\n");

    int posicion = 1;

    while (!heap_empty(ranking)) {

        Pelicula* pelicula = heap_pop(ranking);

        printf("%d. ID: %d | Nota: %d | Veces vista: %d\n",
            posicion,
            pelicula->id,
            pelicula->rating,
            pelicula->times_seen
        );

        posicion++;
    }

    printf("\nCantidad de elementos en lista original: %d\n",
           list_size(lista));

    heap_destroy(ranking);
    list_destroy(lista);

    return 0;
}
