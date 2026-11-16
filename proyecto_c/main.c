#include "menu.h"
#include "tdas/map.h"
#include "tdas/list.h"
#include <stdio.h>
#include <string.h>

static int igualesTexto(void *a, void *b) {
    return strcmp((const char *)a, (const char *)b) == 0;
}

static unsigned long hashTexto(void *clave) {
    const unsigned char *s = (const unsigned char *)clave;
    unsigned long hash = 5381;
    while (*s) hash = ((hash << 5) + hash) + *s++;
    return hash;
}

static int menorEntero(void *a, void *b) {
    return *(const int *)a < *(const int *)b;
}

int main(void) {
    Map *pelis_bytitle = map_create_hash(igualesTexto, hashTexto);
    Map *pelis_byyear = sorted_map_create(menorEntero);
    List *lista_ingreso = list_create();

    if (!pelis_bytitle || !pelis_byyear || !lista_ingreso) {
        fprintf(stderr, "No se pudo reservar memoria para iniciar.\n");
        map_destroy(pelis_bytitle);
        map_destroy(pelis_byyear);
        list_destroy(lista_ingreso);
        return 1;
    }

    ejecutarMenu(pelis_bytitle, pelis_byyear, lista_ingreso);

    map_destroy(pelis_bytitle);
    map_destroy(pelis_byyear);
    list_destroy(lista_ingreso);
    return 0;
}
