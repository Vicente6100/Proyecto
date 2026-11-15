#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tdas/map.h"

// Estructura de la película (asegúrate de que coincida con la que usan tus compañeros)
typedef struct {
    int id;
    char titulo[100];
    int ano;
    float calificacion;
    char rating;
    int vista;         // 1 si está vista, 0 si no
    int veces_vista;
    char resena[751];  // Máximo 750 caracteres + '\0'
} Pelicula;

void registrarPelicula(Map *pelis_bytitle) {
    char titulo_buscado[100];
    
    printf("\n=== REGISTRAR PELÍCULA ===\n");
    printf("Ingrese el título de la película: ");
    getchar(); // Limpiar el buffer
    fgets(titulo_buscado, sizeof(titulo_buscado), stdin);
    titulo_buscado[strcspn(titulo_buscado, "\n")] = 0; // Quitar el salto de línea

    // Buscamos la película en el mapa usando la función de la librería tdas/map.h
    Pelicula* p = search_map(pelis_bytitle, titulo_buscado);

    if (p == NULL) {
        printf("No se encontró ninguna película con ese título.\n");
        return;
    }

    int opcion;
    do {
        printf("\n--- REGISTRAR: \"%s\" ---\n", p->titulo);
        printf("1. Marcar como vista\n");
        printf("2. Puntuar película\n");
        printf("3. Escribir reseña\n");
        printf("4. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: {
                if (p->vista == 1) {
                    char respuesta;
                    printf("La película ya estaba marcada como vista. ¿Desea registrar una nueva visualización? (s/n): ");
                    scanf(" %c", &respuesta);
                    if (respuesta == 's' || respuesta == 'S') {
                        p->veces_vista++;
                        printf("Película marcada como vista nuevamente. Total de veces vista: %d\n", p->veces_vista);
                    }
                } else {
                    p->vista = 1;
                    p->veces_vista = 1;
                    printf("Película marcada como vista.\n");
                }
                break;
            }
            case 2: {
                if (p->vista == 0) {
                    printf("Primero debe marcar la película como vista (Opción 1).\n");
                    break;
                }
                float nota;
                printf("Ingrese una calificación del 1 al 10: ");
                scanf("%f", ¬a);

                if (nota < 1 || nota > 10) {
                    printf("Calificación inválida, ingrese un valor entre 1 y 10.\n");
                } else {
                    p->calificacion = nota;
                    printf("Calificación guardada correctamente.\n");
                }
                break;
            }
            case 3: {
                if (strlen(p->resena) > 0) {
                    printf("Reseña actual:\n> %s\n", p->resena);
                    char respuesta;
                    printf("¿Desea reemplazar la reseña? (s/n): ");
                    scanf(" %c", &respuesta);
                    if (respuesta != 's' && respuesta != 'S') {
                        break;
                    }
                }
                
                printf("Escriba su reseña (máximo 750 caracteres):\n");
                getchar(); // Limpiar buffer
                fgets(p->resena, sizeof(p->resena), stdin);
                p->resena[strcspn(p->resena, "\n")] = 0; // Quitar salto de línea

                if (strlen(p->resena) > 750) {
                    printf("La reseña supera los 750 caracteres. Sobran %lu caracteres. Por favor, reescríbala.\n", strlen(p->resena) - 750);
                    p->resena[0] = '\0'; 
                } else {
                    printf("Reseña guardada con éxito.\n");
                }
                break;
            }
            case 4:
                printf("Volviendo al menú principal...\n");
                break;
            default:
                printf("Opción no válida. Intente de nuevo.\n");
        }
    } while (opcion != 4);
}
