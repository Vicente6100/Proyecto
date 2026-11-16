
#include <stdio.h>
#include <string.h>
#include "tdas/map.h"
#include "tdas/list.h"
#include "pelicula.h"

void registrarPelicula(Map *pelis_bytitle) {
    char titulo_buscado[100];

    printf("\n=== REGISTRAR PELICULA ===\n");
    printf("Ingrese el titulo: ");

    getchar();
    fgets(titulo_buscado, sizeof(titulo_buscado), stdin);
    titulo_buscado[strcspn(titulo_buscado, "\n")] = '\0';

    MapPair *par = map_search(pelis_bytitle, titulo_buscado);

    if (par == NULL) {
        printf("No se encontro la pelicula.\n");
        return;
    }

    List *lista = (List *)par->value;
    Pelicula *p = (Pelicula *)list_first(lista);

    if (p == NULL) {
        printf("No se encontro la pelicula.\n");
        return;
    }

    int opcion;

    do {
        printf("\n--- REGISTRAR: %s ---\n", p->title);
        printf("1. Marcar como vista\n");
        printf("2. Puntuar pelicula\n");
        printf("3. Escribir resena\n");
        printf("4. Salir\n");
        printf("Seleccione una opcion: ");

        scanf("%d", &opcion);

        switch (opcion) {
            case 1: {
                if (p->seen) {
                    char respuesta;

                    printf("La pelicula ya esta marcada como vista. ");
                    printf("Registrar otra visualizacion? (s/n): ");
                    scanf(" %c", &respuesta);

                    if (respuesta == 's' || respuesta == 'S') {
                        p->times_seen++;
                        printf("Visualizaciones: %d\n", p->times_seen);
                    }
                } else {
                    p->seen = 1;
                    p->times_seen = 1;
                    printf("Pelicula marcada como vista.\n");
                }
                break;
            }

            case 2: {
                if (!p->seen) {
                    printf("Primero debe marcar la pelicula como vista.\n");
                    break;
                }

                int nota;

                printf("Ingrese una calificacion del 1 al 10: ");
                scanf("%d", &nota);

                if (nota < 1 || nota > 10) {
                    printf("Calificacion invalida.\n");
                } else {
                    p->rating = nota;
                    printf("Calificacion guardada.\n");
                }
                break;
            }

            case 3: {
                printf("Escriba su resena (maximo 750 caracteres):\n");

                getchar();
                fgets(p->review, sizeof(p->review), stdin);
                p->review[strcspn(p->review, "\n")] = '\0';

                printf("Resena guardada correctamente.\n");
                break;
            }

            case 4:
                printf("Volviendo al menu principal...\n");
                break;

            default:
                printf("Opcion no valida.\n");
        }

    } while (opcion != 4);
}
