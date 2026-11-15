#include "menu.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Funciones que implementan otros integrantes (definidas fuera de este archivo)
void cargarCSV(Map *pelis_bytitle, Map *pelis_byyear, List *lista_ingreso);
void buscarPeliculas(Map *pelis_bytitle, Map *pelis_byyear);
void registrarPelicula(Map *pelis_bytitle);
void mostrarPeliculasVistas(Map *pelis_bytitle, List *lista_ingreso);

static int leerOpcion(int minimo, int maximo) {
    char buffer[64];
    char *fin;

    while (true) {
        printf("Seleccione una opción: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) return 0;

        bool completa = (strchr(buffer, '\n') != NULL);
        if (!completa) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }

        long valor = strtol(buffer, &fin, 10);
        bool es_numero = (fin != buffer && (*fin == '\n' || *fin == '\0'));

        if (completa && es_numero && valor >= minimo && valor <= maximo)
            return (int)valor;

        printf("Opción no válida\n");
    }
}

static void mostrarMenu(void) {
    printf("\n===== REGISTRO DE PELÍCULAS =====\n");
    printf("1. Preparar CSV\n");
    printf("2. Buscar películas\n");
    printf("3. Registrar película\n");
    printf("4. Mostrar películas vistas\n");
    printf("0. Salir\n\n");
}

void ejecutarMenu(Map *pelis_bytitle, Map *pelis_byyear, List *lista_ingreso) {
    bool csv_cargado = false;
    int opcion;

    do {
        mostrarMenu();
        opcion = leerOpcion(0, 4);

        if (opcion >= 2 && opcion <= 4 && !csv_cargado) {
            printf("Primero debe preparar el CSV (opción 1).\n");
            continue;
        }

        switch (opcion) {
        case 1:
            if (csv_cargado) {
                printf("El CSV ya está cargado.\n");
            } else {
                cargarCSV(pelis_bytitle, pelis_byyear, lista_ingreso);
                csv_cargado = true;
            }
            break;
        case 2:
            buscarPeliculas(pelis_bytitle, pelis_byyear);
            break;
        case 3:
            registrarPelicula(pelis_bytitle);
            break;
        case 4:
            mostrarPeliculasVistas(pelis_bytitle, lista_ingreso);
            break;
        case 0:
            printf("Saliendo...\n");
            break;
        }
    } while (opcion != 0);
}