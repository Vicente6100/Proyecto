#include "extra.h"
#include <stdlib.h>
#include <string.h>

/*
 * Lee un registro CSV completo, aunque supere 1024 caracteres.
 * Separa por el delimitador indicado y respeta campos entrecomillados
 * y comillas escapadas ("").
 * Los campos son validos hasta la siguiente llamada.
 */
char **leer_linea_csv(FILE *archivo, char separador) {
    static char *linea = NULL;
    static size_t capacidad = 0;
    static char *campos[300];
    size_t largo = 0;
    int ch;

    if (archivo == NULL) return NULL;

    while ((ch = fgetc(archivo)) != EOF) {
        if (ch == '\n') break;
        if (largo + 2 > capacidad) {
            size_t nueva = capacidad ? capacidad * 2 : 2048;
            char *tmp = realloc(linea, nueva);
            if (tmp == NULL) return NULL;
            linea = tmp;
            capacidad = nueva;
        }
        linea[largo++] = (char)ch;
    }

    if (ch == EOF && largo == 0) return NULL;
    if (largo > 0 && linea[largo - 1] == '\r') largo--;
    linea[largo] = '\0';

    size_t lectura = 0, escritura = 0;
    int cantidad = 0;

    while (1) {
        if (cantidad >= 299) break;
        campos[cantidad++] = &linea[escritura];

        if (linea[lectura] == '"') {
            lectura++;
            while (linea[lectura] != '\0') {
                if (linea[lectura] == '"') {
                    if (linea[lectura + 1] == '"') {
                        linea[escritura++] = '"';
                        lectura += 2;
                    } else {
                        lectura++;
                        break;
                    }
                } else {
                    linea[escritura++] = linea[lectura++];
                }
            }
            while (linea[lectura] != '\0' && linea[lectura] != separador)
                lectura++;
        } else {
            while (linea[lectura] != '\0' && linea[lectura] != separador)
                linea[escritura++] = linea[lectura++];
        }

        int hay_mas = (linea[lectura] == separador);
        if (hay_mas) lectura++;
        linea[escritura++] = '\0';

        if (!hay_mas) break;
    }

    campos[cantidad] = NULL;
    return campos;
}

List *split_string(const char *str, const char *delim) {
    if (str == NULL || delim == NULL) return NULL;
    List *result = list_create();
    if (result == NULL) return NULL;

    char *copia = malloc(strlen(str) + 1);
    if (copia == NULL) return result;
    strcpy(copia, str);

    char *token = strtok(copia, delim);
    while (token != NULL) {
        while (*token == ' ') token++;
        size_t n = strlen(token);
        while (n > 0 && token[n - 1] == ' ') token[--n] = '\0';

        char *nuevo = malloc(n + 1);
        if (nuevo == NULL) break;
        memcpy(nuevo, token, n + 1);
        list_pushBack(result, nuevo);
        token = strtok(NULL, delim);
    }

    free(copia);
    return result;
}

void limpiarPantalla(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void presioneTeclaParaContinuar(void) {
    printf("Presione Enter para continuar...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}
