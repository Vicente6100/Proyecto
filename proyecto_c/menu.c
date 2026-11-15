#include "menu.h"
#include "topk.h"
#include "tdas/extra.h"
#include "tdas/pelicula.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CSV_DEFAULT "CSVpeliculas.csv"
#define MAX_PATH_CSV 512

static char archivo_csv_actual[MAX_PATH_CSV] = CSV_DEFAULT;
static int csv_cargado = 0;

static void quitarSalto(char *s) {
    if (!s) return;
    s[strcspn(s, "\r\n")] = '\0';
}

static int leerLinea(const char *prompt, char *buffer, size_t tam) {
    if (prompt) printf("%s", prompt);
    if (!fgets(buffer, (int)tam, stdin)) return 0;
    if (strchr(buffer, '\n') == NULL && !feof(stdin)) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        puts("La entrada es demasiado larga. Intentelo nuevamente.");
        return 0;
    }
    quitarSalto(buffer);
    return 1;
}

static int leerEntero(const char *prompt, int minimo, int maximo, int *valor) {
    char buffer[128], *fin;
    for (;;) {
        if (!leerLinea(prompt, buffer, sizeof(buffer))) return 0;
        errno = 0;
        long n = strtol(buffer, &fin, 10);
        while (*fin == ' ' || *fin == '\t') fin++;
        if (fin != buffer && *fin == '\0' && errno == 0 && n >= minimo && n <= maximo) {
            *valor = (int)n;
            return 1;
        }
        printf("Entrada invalida. Ingrese un numero entre %d y %d.\n", minimo, maximo);
    }
}

static int copiarCampo(char *destino, size_t capacidad, const char *origen) {
    size_t n = strlen(origen);
    if (n >= capacidad) return 0;
    memcpy(destino, origen, n + 1);
    return 1;
}

static int convertirDecimal(const char *s, double *valor) {
    if (s == NULL || *s == '\0') { *valor = 0.0; return 1; }
    char *fin;
    errno = 0;
    double n = strtod(s, &fin);
    if (s == fin || *fin != '\0' || errno != 0) return 0;
    *valor = n;
    return 1;
}

static int convertirEntero(const char *s, int *valor) {
    char *fin;
    errno = 0;
    long n = strtol(s, &fin, 10);
    if (s == fin || *fin != '\0' || errno != 0 || n < -2147483647L || n > 2147483647L)
        return 0;
    *valor = (int)n;
    return 1;
}

static void agregarPelicula(Map *porTitulo, Map *porAnio, List *ingreso, Pelicula *p) {
    map_insert(porTitulo, p->title, p);
    MapPair *par = map_search(porAnio, &p->release_year);
    List *listaAnio;
    if (!par) {
        listaAnio = list_create();
        if (!listaAnio) return;
        map_insert(porAnio, &p->release_year, listaAnio);
    } else {
        listaAnio = (List *)par->value;
    }
    list_pushBack(listaAnio, p);
    list_pushBack(ingreso, p);
}

static void liberarDatos(Map *porTitulo, Map *porAnio, List *ingreso) {
    for (Pelicula *p = list_first(ingreso); p; p = list_next(ingreso))
        free(p);
    MapPair *par = map_first(porAnio);
    while (par) {
        list_destroy((List *)par->value);
        par = map_next(porAnio);
    }
    map_clean(porTitulo);
    map_clean(porAnio);
    list_clean(ingreso);
    csv_cargado = 0;
}

static int cargarCSV(Map *porTitulo, Map *porAnio, List *ingreso, const char *nombre) {
    FILE *f = fopen(nombre, "r");
    if (!f) {
        printf("No se pudo abrir el archivo: %s\n", nombre);
        return 0;
    }

    char **campos = leer_linea_csv(f, ';'); /* encabezado */
    if (!campos) {
        puts("El archivo CSV esta vacio.");
        fclose(f);
        return 0;
    }

    int cargadas = 0, omitidas = 0;
    while ((campos = leer_linea_csv(f, ';')) != NULL) {
        int cantidad = 0;
        while (campos[cantidad] != NULL) cantidad++;
        if (cantidad != 12) {
            omitidas++;
            continue;
        }

        Pelicula *p = calloc(1, sizeof(Pelicula));
        if (!p) {
            fclose(f);
            puts("Memoria insuficiente al cargar el CSV.");
            return 0;
        }

        int valido =
            convertirEntero(campos[0], &p->column1) &&
            convertirEntero(campos[1], &p->id) &&
            copiarCampo(p->title, sizeof(p->title), campos[2]) &&
            copiarCampo(p->overview, sizeof(p->overview), campos[3]) &&
            convertirEntero(campos[4], &p->release_year) &&
            convertirEntero(campos[5], &p->popularity) &&
            convertirDecimal(campos[6], &p->vote_average) &&
            convertirEntero(campos[7], &p->vote_count) &&
            convertirEntero(campos[8], &p->seen) &&
            convertirEntero(campos[9], &p->times_seen) &&
            convertirEntero(campos[10], &p->rating) &&
            copiarCampo(p->review, sizeof(p->review), campos[11]);

        if (!valido || p->title[0] == '\0' || p->release_year < 0 ||
            (p->seen != 0 && p->seen != 1) || p->times_seen < 0 ||
            p->rating < 0 || p->rating > 10) {
            free(p);
            omitidas++;
            continue;
        }

        if (map_search(porTitulo, p->title) != NULL) {
            free(p);
            omitidas++;
            continue;
        }

        agregarPelicula(porTitulo, porAnio, ingreso, p);
        cargadas++;
    }

    fclose(f);
    if (cargadas == 0) {
        puts("No se cargaron peliculas validas.");
        return 0;
    }

    if (!copiarCampo(archivo_csv_actual, sizeof(archivo_csv_actual), nombre))
        strcpy(archivo_csv_actual, CSV_DEFAULT);
    csv_cargado = 1;

    printf("Archivo cargado correctamente: %d peliculas.", cargadas);
    if (omitidas) printf(" Registros omitidos: %d.", omitidas);
    putchar('\n');
    return 1;
}

static void escribirCampo(FILE *f, const char *s) {
    fputc('"', f);
    for (const char *p = s; *p; p++) {
        if (*p == '"') fputc('"', f);
        fputc(*p, f);
    }
    fputc('"', f);
}

static int guardarCSVConLista(List *ingreso) {
    if (!csv_cargado) return 1;
    char temporal[MAX_PATH_CSV + 16];
    snprintf(temporal, sizeof(temporal), "%s.tmp", archivo_csv_actual);
    FILE *f = fopen(temporal, "w");
    if (!f) {
        printf("No se pudo guardar el CSV.\n");
        return 0;
    }
    fputs("Column1;id;title;overview;release_year;popularity;vote_average;vote_count;seen;times_seen;rating;review\n", f);
    for (Pelicula *p = list_first(ingreso); p; p = list_next(ingreso)) {
        fprintf(f, "%d;%d;", p->column1, p->id);
        escribirCampo(f, p->title); fputc(';', f);
        escribirCampo(f, p->overview);
        fprintf(f, ";%d;%d;%.3f;%d;%d;%d;%d;", p->release_year, p->popularity,
                p->vote_average, p->vote_count, p->seen, p->times_seen, p->rating);
        escribirCampo(f, p->review);
        fputc('\n', f);
    }
    int correcto = !ferror(f);
    if (fclose(f) != 0) correcto = 0;
    if (!correcto) {
        remove(temporal);
        puts("Error al escribir el CSV; se conservo el archivo original.");
        return 0;
    }
    char respaldo[MAX_PATH_CSV + 16];
    snprintf(respaldo, sizeof(respaldo), "%s.bak", archivo_csv_actual);
    remove(respaldo);
    if (rename(archivo_csv_actual, respaldo) != 0) {
        remove(temporal);
        puts("No se pudo respaldar el CSV original.");
        return 0;
    }
    if (rename(temporal, archivo_csv_actual) != 0) {
        (void)rename(respaldo, archivo_csv_actual);
        remove(temporal);
        puts("No se pudo reemplazar el CSV; se restauro el original.");
        return 0;
    }
    remove(respaldo);
    puts("Cambios guardados correctamente.");
    return 1;
}

static void mostrarFicha(const Pelicula *p) {
    printf("\nID: %d\nTitulo: %s\nAno: %d\nEstado: %s\nVeces vista: %d\n",
           p->id, p->title, p->release_year, p->seen ? "Vista" : "Pendiente", p->times_seen);
    printf("Calificacion: %s", p->rating ? "" : "Sin calificar\n");
    if (p->rating) printf("%d/10\n", p->rating);
    printf("Resena: %s\n", p->review);
}

static void buscarPorNombre(Map *porTitulo) {
    char titulo[256];
    if (!leerLinea("Ingrese el titulo: ", titulo, sizeof(titulo))) return;
    MapPair *par = map_search(porTitulo, titulo);
    Pelicula *p = par ? (Pelicula *)par->value : NULL;
    if (!p) puts("No se encontro ninguna pelicula con ese titulo.");
    else mostrarFicha(p);
}

static void buscarPorAnio(Map *porAnio) {
    int anio;
    if (!leerEntero("Ingrese el ano: ", 1880, 2100, &anio)) return;
    MapPair *par = map_search(porAnio, &anio);
    if (!par) {
        puts("No hay peliculas de ese ano.");
        return;
    }
    List *lista = (List *)par->value;
    int n = 0;
    for (Pelicula *p = list_first(lista); p; p = list_next(lista)) {
        printf("ID: %d | %s | %s\n", p->id, p->title, p->seen ? "Vista" : "Pendiente");
        n++;
    }
    printf("Total: %d peliculas.\n", n);
}

static void menuBuscar(Map *porTitulo, Map *porAnio) {
    int op;
    do {
        puts("\n--- BUSCAR PELICULAS ---\n1. Buscar por nombre\n2. Buscar por ano de lanzamiento\n0. Volver");
        if (!leerEntero("Seleccione una opcion: ", 0, 2, &op)) return;
        if (op == 1) buscarPorNombre(porTitulo);
        if (op == 2) buscarPorAnio(porAnio);
    } while (op != 0);
}

static Pelicula *seleccionarPelicula(Map *porTitulo) {
    char titulo[256];
    if (!leerLinea("Ingrese el titulo de la pelicula: ", titulo, sizeof(titulo))) return NULL;
    MapPair *par = map_search(porTitulo, titulo);
    if (!par) {
        puts("No se encontro ninguna pelicula con ese titulo.");
        return NULL;
    }
    Pelicula *p = (Pelicula *)par->value;
    mostrarFicha(p);
    return p;
}

static void registrarPelicula(Map *porTitulo, List *ingreso) {
    Pelicula *p = seleccionarPelicula(porTitulo);
    if (!p) return;
    int op;
    do {
        puts("\n--- REGISTRAR PELICULA ---\n1. Marcar como vista\n2. Puntuar pelicula\n3. Escribir resena\n4. Salir");
        if (!leerEntero("Seleccione una opcion: ", 1, 4, &op)) return;
        if (op == 1) {
            if (p->seen) {
                int nueva;
                if (!leerEntero("Ya esta marcada como vista. ¿Registrar otra visualizacion? (1=Si, 0=No): ", 0, 1, &nueva))
                    continue;
                if (!nueva) continue;
            }
            p->seen = 1;
            p->times_seen++;
            puts("Pelicula marcada como vista.");
            guardarCSVConLista(ingreso);
        } else if (op == 2) {
            if (!p->seen) {
                puts("Solo se pueden puntuar peliculas vistas. Marque la pelicula como vista primero.");
                continue;
            }
            int nota;
            if (leerEntero("Ingrese una calificacion entre 1 y 10: ", 1, 10, &nota)) {
                p->rating = nota;
                puts("Calificacion guardada.");
                guardarCSVConLista(ingreso);
            }
        } else if (op == 3) {
            if (strcmp(p->review, "Not reviewed") != 0 && p->review[0] != '\0') {
                printf("Resena actual: %s\n", p->review);
                int reemplazar;
                if (!leerEntero("¿Desea reemplazarla? (1=Si, 0=No): ", 0, 1, &reemplazar) || !reemplazar)
                    continue;
            }
            char resena[2048];
            do {
                if (!leerLinea("Escriba su resena (maximo 750 caracteres): ", resena, sizeof(resena)))
                    return;
                if (strlen(resena) > 750) {
                    printf("La resena supera el limite por %zu caracteres. Intentelo nuevamente.\n",
                           strlen(resena) - 750);
                }
            } while (strlen(resena) > 750);
            strcpy(p->review, resena);
            puts("Resena guardada.");
            guardarCSVConLista(ingreso);
        }
    } while (op != 4);
}

static void mostrarTodasVistas(List *ingreso) {
    int n = 0;
    for (Pelicula *p = list_first(ingreso); p; p = list_next(ingreso)) {
        if (!p->seen) continue;
        printf("%s (%d) | %s | Veces vista: %d\n", p->title, p->release_year,
               p->rating ? "Calificada" : "Sin calificar", p->times_seen);
        if (p->rating) printf("  Calificacion: %d/10\n", p->rating);
        n++;
    }
    if (!n) puts("Aun no han registrado peliculas vistas.");
    else printf("Total de peliculas vistas: %d\n", n);
}

static void consultarVista(Map *porTitulo) {
    Pelicula *p = seleccionarPelicula(porTitulo);
    if (!p) return;
    if (!p->seen) {
        puts("Esta pelicula aun no esta marcada como vista. Use la opcion 3 del menu principal.");
        return;
    }
    mostrarFicha(p);
}

static void menuVistas(Map *porTitulo, List *ingreso) {
    int op;
    do {
        puts("\n--- PELICULAS VISTAS ---\n1. Ver Top de peliculas\n2. Ver todas las peliculas vistas\n3. Consultar una pelicula vista\n4. Salir");
        if (!leerEntero("Seleccione una opcion: ", 1, 4, &op)) return;
        if (op == 1) mostrarTopK(ingreso);
        else if (op == 2) mostrarTodasVistas(ingreso);
        else if (op == 3) consultarVista(porTitulo);
    } while (op != 4);
}

void ejecutarMenu(Map *pelis_bytitle, Map *pelis_byyear, List *lista_ingreso) {
    int opcion;
    do {
        puts("\n===== REGISTRO DE PELICULAS =====\n1. Preparar CSV\n2. Buscar peliculas\n3. Registrar pelicula\n4. Mostrar peliculas vistas\n0. Salir");
        if (!leerEntero("Seleccione una opcion: ", 0, 4, &opcion)) break;

        if (opcion >= 2 && opcion <= 4 && !csv_cargado) {
            puts("Primero debe preparar el CSV (opcion 1).");
            continue;
        }

        switch (opcion) {
        case 1: {
            if (csv_cargado) {
                puts("El CSV ya esta cargado.");
                break;
            }
            char nombre[MAX_PATH_CSV];
            if (!leerLinea("Nombre del archivo CSV (Enter para CSVpeliculas.csv): ", nombre, sizeof(nombre)))
                break;
            if (nombre[0] == '\0') strcpy(nombre, CSV_DEFAULT);
            if (cargarCSV(pelis_bytitle, pelis_byyear, lista_ingreso, nombre))
                puts("Menu listo.");
            break;
        }
        case 2: menuBuscar(pelis_bytitle, pelis_byyear); break;
        case 3: registrarPelicula(pelis_bytitle, lista_ingreso); break;
        case 4: menuVistas(pelis_bytitle, lista_ingreso); break;
        case 0:
            if (csv_cargado) guardarCSVConLista(lista_ingreso);
            puts("Saliendo...");
            break;
        }
    } while (opcion != 0);

    if (csv_cargado) liberarDatos(pelis_bytitle, pelis_byyear, lista_ingreso);
}
