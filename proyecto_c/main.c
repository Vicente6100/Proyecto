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
    int calification ;
    char review[751] ;
    
} Pelicula ;

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
    
}

int main() {
    //char opcion ;

    Map *pelis_bytitle = map_create(is_equal_str) ;
    Map *pelis_byyear = sorted_map_create(is_equal_int) ;
    cargarCSV(pelis_bytitle, pelis_byyear) ; 
    return 0;
}   
