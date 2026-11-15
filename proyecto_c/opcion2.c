#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opcion2.h"
#include "pelicula.h"
#include "tdas/list.h"
#include "tdas/map.h"

#define ENTRADA_MAX   256   /* buffer de lectura; mayor que cualquier título válido */
#define LINEAS_PAGINA 25    /* películas mostradas antes de pausar la lista por año */

/* ------------------------------------------------------------------ */
/*  Utilidades de entrada                                              */
/* ------------------------------------------------------------------ */

/* Lee una línea completa sin desbordar el buffer. Devuelve false solo al
   llegar al fin de la entrada (EOF). Si la línea no cabe, descarta el resto. */
static bool leer_linea(char *destino, size_t tam)
{
    if (fgets(destino, (int)tam, stdin) == NULL) {
        destino[0] = '\0';
        return false;
    }

    size_t largo = strlen(destino);
    if (largo > 0 && destino[largo - 1] == '\n') {
        destino[--largo] = '\0';
        if (largo > 0 && destino[largo - 1] == '\r') {
            destino[--largo] = '\0';
        }
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
            /* se descarta el resto de la línea */
        }
    }
    return true;
}

/* Quita espacios al inicio y al final. */
static void recortar(char *s)
{
    size_t ini = 0;
    size_t fin = strlen(s);

    while (s[ini] != '\0' && isspace((unsigned char)s[ini])) {
        ini++;
    }
    while (fin > ini && isspace((unsigned char)s[fin - 1])) {
        fin--;
    }
    memmove(s, s + ini, fin - ini);
    s[fin - ini] = '\0';
}

/* Convierte un texto a entero. Falla si está vacío, tiene letras o no cabe en un int. */
static bool convertir_entero(const char *texto, int *valor)
{
    if (texto[0] == '\0') {
        return false;
    }

    char *resto;
    errno = 0;
    long n = strtol(texto, &resto, 10);

    if (errno != 0 || *resto != '\0' || n < INT_MIN || n > INT_MAX) {
        return false;
    }
    *valor = (int)n;
    return true;
}

/* Pregunta s/n hasta recibir una respuesta válida. */
static bool preguntar_reintentar(void)
{
    char respuesta[ENTRADA_MAX];

    for (;;) {
        printf("¿Desea intentar de nuevo? (s/n): ");
        if (!leer_linea(respuesta, sizeof respuesta)) {
            return false;
        }
        recortar(respuesta);

        if (strlen(respuesta) == 1) {
            char r = (char)tolower((unsigned char)respuesta[0]);
            if (r == 's') return true;
            if (r == 'n') return false;
        }
        printf("Responda 's' o 'n'.\n");
    }
}

/* ------------------------------------------------------------------ */
/*  Presentación                                                       */
/* ------------------------------------------------------------------ */

static void mostrar_ficha(const Pelicula *p)
{
    printf("\n--- FICHA DE LA PELÍCULA ---\n");
    printf("ID:           %d\n", p->id);
    printf("Título:       %s\n", p->title);
    printf("Año:          %d\n", p->release_year);

    if (p->calification > 0) {
        printf("Calificación: %d/10\n", p->calification);
    } else {
        printf("Calificación: sin calificar\n");
    }

    if (p->seen) {
        printf("Vista:        Sí (%d %s)\n", p->times_seen,
               p->times_seen == 1 ? "vez" : "veces");
    } else {
        printf("Vista:        No\n");
    }

    /* El CSV trae "Not reviewed" cuando no hay reseña */
    bool sin_resena = p->review[0] == '\0' || strcmp(p->review, "Not reviewed") == 0;
    printf("Reseña:       %s\n", sin_resena ? "(sin reseña)" : p->review);
}

/* ------------------------------------------------------------------ */
/*  Buscar por nombre                                                  */
/*  map_search sobre pelis_bytitle -> List de las películas con ese     */
/*  título (casi siempre una; varias si hay remakes con el mismo nombre).*/
/*  Costo: O(1) esperado para llegar a la lista + O(k) para mostrar sus */
/*  k películas.                                                        */
/* ------------------------------------------------------------------ */

static void buscar_por_nombre(Map *pelis_bytitle)
{
    char titulo[ENTRADA_MAX];

    for (;;) {
        printf("\nIngrese el título de la película (vacío para volver): ");
        if (!leer_linea(titulo, sizeof titulo)) {
            return;
        }
        recortar(titulo);

        if (titulo[0] == '\0') {
            return;
        }

        if (strlen(titulo) >= sizeof(((Pelicula *)0)->title)) {
            printf("El título es demasiado largo (máximo %zu caracteres).\n",
                   sizeof(((Pelicula *)0)->title) - 1);
        } else {
            MapPair *par = map_search(pelis_bytitle, titulo);
            List *lista = (par != NULL) ? (List *)par->value : NULL;

            int coincidencias = 0;
            for (Pelicula *p = (Pelicula *)list_first(lista); p != NULL;
                 p = (Pelicula *)list_next(lista)) {
                mostrar_ficha(p);
                coincidencias++;
            }

            if (coincidencias > 0) {
                if (coincidencias > 1) {
                    printf("\nSe encontraron %d películas con ese título.\n", coincidencias);
                }
                return;
            }
            printf("No se encontró ninguna película con ese título\n");
        }

        if (!preguntar_reintentar()) {
            return;
        }
    }
}

/* ------------------------------------------------------------------ */
/*  Buscar por año de lanzamiento                                      */
/*  map_search sobre pelis_byyear -> List de películas del año.         */
/*  Costo: O(1) esperado para llegar a la lista + O(m) para mostrar sus */
/*  m películas.                                                        */
/* ------------------------------------------------------------------ */

static void buscar_por_anio(Map *pelis_byyear)
{
    char entrada[ENTRADA_MAX];
    int anio = 0;

    for (;;) {
        printf("\nIngrese el año de lanzamiento (vacío para volver): ");
        if (!leer_linea(entrada, sizeof entrada)) {
            return;
        }
        recortar(entrada);

        if (entrada[0] == '\0') {
            return;
        }
        if (convertir_entero(entrada, &anio) && anio > 0) {
            break;
        }
        printf("Año inválido, ingrese un número entero (por ejemplo, 1994).\n");
    }

    int clave = anio;
    MapPair *par = map_search(pelis_byyear, &clave);
    List *lista = (par != NULL) ? (List *)par->value : NULL;
    Pelicula *p = (lista != NULL) ? (Pelicula *)list_first(lista) : NULL;

    if (p == NULL) {
        printf("No hay películas registradas del año %d.\n", anio);
        return;
    }

    printf("\n--- PELÍCULAS DEL AÑO %d (%d en total) ---\n", anio, list_size(lista));

    int mostradas = 0;
    while (p != NULL) {
        printf("[%d] %s - %s\n", p->id, p->title, p->seen ? "vista" : "pendiente");
        mostradas++;
        p = (Pelicula *)list_next(lista);

        /* Pausa cada LINEAS_PAGINA para que la lista no se pase de largo en la consola */
        if (p != NULL && mostradas % LINEAS_PAGINA == 0) {
            char resp[ENTRADA_MAX];
            printf("-- Enter para ver más, 'q' para terminar --");
            if (!leer_linea(resp, sizeof resp)) {
                return;
            }
            recortar(resp);
            if (resp[0] == 'q' || resp[0] == 'Q') {
                return;
            }
        }
    }
}

/* ------------------------------------------------------------------ */
/*  Submenú                                                            */
/* ------------------------------------------------------------------ */

void opcion_buscar_peliculas(Map *pelis_bytitle, Map *pelis_byyear)
{
    char entrada[ENTRADA_MAX];

    for (;;) {
        printf("\n--- BUSCAR PELÍCULAS ---\n");
        printf("1. Buscar por nombre\n");
        printf("2. Buscar por año de lanzamiento\n");
        printf("0. Volver\n");
        printf("Seleccione una opción: ");

        if (!leer_linea(entrada, sizeof entrada)) {
            return;
        }
        recortar(entrada);

        int opcion;
        if (!convertir_entero(entrada, &opcion)) {
            opcion = -1;
        }

        switch (opcion) {
            case 1:  buscar_por_nombre(pelis_bytitle); break;
            case 2:  buscar_por_anio(pelis_byyear);    break;
            case 0:  return;
            default: printf("Opción no válida\n");
        }
    }
}
