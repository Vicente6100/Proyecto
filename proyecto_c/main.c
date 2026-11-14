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

FILE* abrirCSV(const char *nombre_csv, const char *modo){
    FILE *archivo = fopen(nombre_csv, modo) ;
    if (archivo == NULL ){
        perror("Error al abrir el CSV "%s" ", nombre_csv ) ;
        exit(1) ;
    }
    return archivo ;
}

void cargarCSV(Map *pelis_bytitle, Map *pelis_byyear ){
    const char nombre_csv = "CSVpeliculas.csv" ;
    const char modo = "r" ;
    archivo = abrirCSV(nombre_csv, modo) ;

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

        MapPair *title_pair = map_search(pelis_bytitle, pelicula->title) ;
        if (title_pair == NULL){
            List *nueva_lista = list_create() ;
            list_pushBack(nueva_lista, pelicula) ;
            map_insert(pelis_bytitle, pelicula->title, nueva_lista) ;
        }
        else{
            List *title_list = (List *) title_pair->value;
            list_pushBack(title_list, pelicula) ;
        }

        Map *year_pair = map_search(pelis_byyear, pelicula->year) ;
        if (title_pair == NULL){
            List *nueva_lista = list_create() ;
            list_pushBack(nueva_lista, pelicula) ;
            map_insert(pelis_byyear, pelicula->year, nueva_lista) ;
        }
        else{
            List *year_list = (List *) year_pair->value;
            list_pushBack(year_list, pelicula) ;
        }

        fclose(archivo)

    }
    printf("El archivo fue cargado con éxito") ;
}

int main() {
    //char opcion ;

    Map *pelis_bytitle = map_create(is_equal_str) ;
    Map *pelis_byyear = sorted_map_create(is_equal_int) ;
    cargarCSV(pelis_bytitle, pelis_byyear) ; 
    return 0;
}   
