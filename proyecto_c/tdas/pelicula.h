#ifndef PELICULA_H
#define PELICULA_H

typedef struct {
    int column1;
    int id;
    char title[100];
    char overview[1200];
    int release_year;
    int popularity;
    double vote_average;
    int vote_count;
    int seen;
    int times_seen;
    int rating;          /* 0 = sin calificar; 1 a 10 = nota del usuario */
    char review[751];    /* maximo 750 caracteres mas '\0' */
} Pelicula;

#endif
