#include "topk.h"
#include "tdas/heap.h"
#include "tdas/pelicula.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int compararPeliculasTopK(void *a, void *b) {
    const Pelicula *p1 = (const Pelicula *)a;
    const Pelicula *p2 = (const Pelicula *)b;

    if (p1->rating != p2->rating)
        return p1->rating > p2->rating ? 1 : -1;
    if (p1->times_seen != p2->times_seen)
        return p1->times_seen > p2->times_seen ? 1 : -1;
    return 0;
}

static int leerK(int *k) {
    char buffer[64], *fin;
    printf("¿Cuantas peliculas deseas ver?: ");
    if (!fgets(buffer, sizeof(buffer), stdin)) return 0;
    long valor = strtol(buffer, &fin, 10);
    while (*fin == ' ' || *fin == '\t') fin++;
    if (fin == buffer || (*fin != '\n' && *fin != '\0') || valor <= 0 || valor > 100000)
        return 0;
    *k = (int)valor;
    return 1;
}

void mostrarTopK(List *lista_ingreso) {
    if (!lista_ingreso) {
        puts("Primero debe preparar el CSV.");
        return;
    }

    List *calificadas = list_create();
    if (!calificadas) {
        puts("No hay memoria suficiente para generar el ranking.");
        return;
    }

    int cantidad = 0;
    for (Pelicula *p = list_first(lista_ingreso); p; p = list_next(lista_ingreso)) {
        if (p->seen && p->rating >= 1 && p->rating <= 10) {
            list_pushBack(calificadas, p);
            cantidad++;
        }
    }

    if (cantidad == 0) {
        puts("Aun no ha calificado ninguna pelicula.");
        list_destroy(calificadas);
        return;
    }

    printf("\nHay %d peliculas calificadas.\n", cantidad);
    int k;
    if (!leerK(&k)) {
        puts("Cantidad invalida. Ingrese un entero positivo.");
        list_destroy(calificadas);
        return;
    }
    if (k > cantidad) k = cantidad;

    Heap *ranking = heap_create(compararPeliculasTopK);
    if (!ranking || !heap_build(ranking, calificadas)) {
        puts("No se pudo construir el ranking.");
        heap_destroy(ranking);
        list_destroy(calificadas);
        return;
    }

    printf("\n========== TOP %d DE PELICULAS ==========\n", k);
    for (int i = 0; i < k; i++) {
        Pelicula *p = heap_pop(ranking);
        if (!p) break;
        printf("%d. %s (%d) | Nota: %d/10 | Vista %d vez/veces\n",
               i + 1, p->title, p->release_year, p->rating, p->times_seen);
    }
    puts("=========================================");

    heap_destroy(ranking);
    list_destroy(calificadas);
}
