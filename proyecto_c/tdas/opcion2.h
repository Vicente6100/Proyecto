#ifndef OPCION2_H
#define OPCION2_H

#include "tdas/map.h"

/*
 * Opción 2 del menú principal: submenú "Buscar películas".
 *   pelis_bytitle: Map (tabla hash) título (char *) -> List de Pelicula con ese título
 *   pelis_byyear : Map (tabla hash) año (int *)     -> List de Pelicula de ese año
 * El main debe llamarla solo después de cargar el CSV.
 */
void opcion_buscar_peliculas(Map *pelis_bytitle, Map *pelis_byyear);

#endif
