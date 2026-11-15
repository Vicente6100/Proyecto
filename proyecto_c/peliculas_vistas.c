
#include "peliculas_vistas.h"
#include "pelicula.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int compararTitulos(const char *titulo1, const char *titulo2) {
    while (*titulo1 && *titulo2) {
        if (tolower((unsigned char)*titulo1) !=
            tolower((unsigned char)*titulo2)) {
            return 0;
        }
        titulo1++;
        titulo2++;
    }
    return *titulo1 == '\0' && *titulo2 == '\0';
}

void imprimirPelicula(Pelicula *pelicula) {
    printf("\nTitulo: %s\n", pelicula->titulo);
    printf("Ano: %d\n", pelicula->ano);
    printf("Veces vista: %d\n", pelicula->veces_vista);
    printf("Calificacion: %.1f\n", pelicula->calificacion);
    printf("Resena: %s\n", pelicula->resena);
}

void mostrarPeliculasVistas(List *lista_ingreso) {
    int encontradas = 0;

    printf("\n===== PELICULAS VISTAS =====\n");

    Pelicula *pelicula = list_first(lista_ingreso);

    while (pelicula != NULL) {
        if (pelicula->vista == 1) {
            imprimirPelicula(pelicula);
            encontradas++;
        }

        pelicula = list_next(lista_ingreso);
    }

    if (encontradas == 0) {
        printf("\nNo hay peliculas vistas registradas.\n");
    }
}

void consultarPeliculaVista(List *lista_ingreso) {
    char titulo[100];

    printf("\nIngrese el titulo de la pelicula: ");

    if (fgets(titulo, sizeof(titulo), stdin) == NULL) {
        return;
    }

    titulo[strcspn(titulo, "\n")] = '\0';

    int encontrada = 0;
    Pelicula *pelicula = list_first(lista_ingreso);

    while (pelicula != NULL) {
        if (pelicula->vista == 1 &&
            compararTitulos(pelicula->titulo, titulo)) {
            imprimirPelicula(pelicula);
            encontrada = 1;
        }

        pelicula = list_next(lista_ingreso);
    }

    if (encontrada == 0) {
        printf("\nNo se encontro una pelicula vista con ese titulo.\n");
    }
}
