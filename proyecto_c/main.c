#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct{
    int id ;
    char title[100] ;
    int release_year ;
    int seen ; //trabajado como booleano, es 0 o 1
    int times_seen ;
    int rating ;
    char review[751] ;
    
} tipoPelicula ;

int is_equal_str(void *key1, void *key2){
    return strcmp( (char *)key1, (char *)key2 ) == 0 ;
}

int is_equal_int(void *key1, void *key2){
    return *(int *)key1 == *(int *)key2;
}

void cargarCSV(Map *pelis_bytitle, Map *pelis_byyear ){
    FILE *archivo = fopen("CSVpeliculas.csv", "r") ;
    if (archivo == NULL ){
        perror("Error al abrir el CSV") ;
        return ;
    }

    char **campos ;
    campos = leer_linea_csv(archivo, ',') ;
    while ((campos = leer_linea_csv(archivo, ',')) != NULL){
        tipoPelicula *pelicula = (tipoPelicula *)malloc(sizeof(tipoPelicula)) ;
        pelicula->id = atoi(campos[1]) ; 
        strcpy(pelicula->title, campos[3]) ;
        pelicula->release_year = atoi(campos[4]) ;
        pelicula->seen = atoi(campos[7]) ;
        pelicula->times_seen = atoi(campos[9]) ;
        pelicula->rating = atoi(campos[10]) ;
        strcpy(pelicula->review, campos[11] ) :

        

    }
}

int main() {
    //char opcion ;

    Map *pelis_bytitle = map_create(is_equal_str) ;
    Map *pelis_byyear = sorted_map_create(is_equal_int) ;
    cargarCSV(pelis_bytitle, pelis_byyear) ; 
    return 0;
}   
