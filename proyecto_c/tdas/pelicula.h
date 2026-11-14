#ifndef PELICULA_H
#define PELICULA_H

/* Entidad Película. Se movió aquí desde main.c para que la usen todos los .c */
typedef struct {
    int  id;
    char title[100];
    int  release_year;
    int  seen;            /* trabajado como booleano: 0 o 1 */
    int  times_seen;
    int  calification;    /* 0 = sin calificar; 1 a 10 = nota del usuario */
    char review[751];
} Pelicula;

#endif
