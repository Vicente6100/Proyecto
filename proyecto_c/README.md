# Registro de Peliculas

Aplicacion de consola en C para administrar un registro local de peliculas usando un archivo CSV.

## Funcionalidades

1. **Preparar CSV:** carga `CSVpeliculas.csv` (o el archivo indicado por el usuario).
2. **Buscar peliculas:** busqueda por titulo y listado por ano de lanzamiento.
3. **Registrar pelicula:** marcar como vista, registrar nuevas visualizaciones, asignar nota de 1 a 10 y escribir/reemplazar una resena de hasta 750 caracteres.
4. **Mostrar peliculas vistas:** Top K, listado completo de vistas y consulta individual.
5. **Guardar:** los cambios se escriben en un archivo temporal y luego reemplazan el CSV original. Se mantiene el orden de los registros cargados.

## Estructuras de datos

- Mapa hash de peliculas, con el titulo como clave.
- Mapa ordenado de anos, cuyo valor es una lista de peliculas.
- Lista de ingreso, que conserva el orden del CSV.
- Max-Heap generico sobre arreglo dinamico para calcular el Top K. Se construye con el metodo de Floyd en O(C), y cada extraccion cuesta O(log C). En empates se prioriza la pelicula vista mas veces.

## Compilar en Windows (PowerShell, MinGW GCC)

Desde la carpeta `proyecto_c`:

```powershell
gcc -Wall -Wextra -std=c11 main.c menu.c topk.c tdas/heap.c tdas/extra.c tdas/list.c tdas/map.c -o programa.exe
```

Ejecutar:

```powershell
.\programa.exe
```

El archivo `CSVpeliculas.csv` debe estar en la misma carpeta que `programa.exe`, o se puede indicar una ruta al elegir **Preparar CSV**.

## Compilar en Linux

```bash
gcc -Wall -Wextra -std=c11 main.c menu.c topk.c tdas/heap.c tdas/extra.c tdas/list.c tdas/map.c -o programa
./programa
```

## Prueba independiente del Max-Heap

```powershell
gcc -Wall -Wextra -std=c11 test_heap.c tdas/heap.c tdas/list.c -o test_heap.exe
.\test_heap.exe
```

## Formato CSV

El programa trabaja con 12 columnas separadas por punto y coma:


`Column1;id;title;overview;release_year;popularity;vote_average;vote_count;seen;times_seen;rating;review`

La lectura respeta campos entrecomillados y comillas dobles escapadas. Las columnas originales que no modifica el registro de peliculas se conservan al guardar.


##EJEMPLO DE USO

PASO 1: Iniciar aplicación 
  -Se abre la carpeta proyecto_c en una terminal
  -Se ejecuta el comando 
  ```powershell
  gcc -Wall -Wextra -std=c11 main.c menu.c topk.c tdas/heap.c tdas/extra.c tdas/list.c tdas/map.c -o programa.exe
```
  -Se ejecuta el comando 
  ```powershell
  .\programa.exe
```

PASO 2: interactuar con el primer menú
  - Se selecciona la opción 1: Preparar CSV
    -Se presiona la tecla Enter
  - Se selecciona la opción 2: Buscar película
  Se abre el submenú
    - Opción 1: Buscar por nombre
      -Se escribe el nombre de la película: Iron Man
      ```powershell
  ID: 1726
Titulo: Iron Man
Ano: 2008
Estado: Pendiente
Veces vista: 0
Calificacion: Sin calificar
Resena: Not reviewed
```
