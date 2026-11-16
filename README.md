# Registro de Películas Vistas (Offline)

Aplicación de consola desarrollada en **C** .Permite gestionar y registrar películas vistas sin necesidad de conexión a internet,
operando sobre una base de datos local en formato CSV con más de 10.000 títulos.

---

## Integrantes del Equipo

* Vicente Briones
* Nicolás Gaete
* Melany Padilla
* Felipe Jeria
* Ian Ugalde
* Trinidad Tejeda

---

## Funcionalidades Principales

1. **Preparar CSV:** Carga el archivo `CSVpeliculas.csv` en memoria, procesando correctamente campos con comas y comillas (como títulos y reseñas) y construyendo las estructuras de datos.
2. **Buscar películas:**
   * **Por nombre:** Búsqueda rápida en tiempo promedio O(1)utilizando una tabla hash.
   * **Por año de lanzamiento:** Consulta de todas las películas estrenadas en un año específico mediante un multimapa.
3. **Registrar película:** Permite buscar un título para:
   * Marcarla como vista e incrementar el contador de visualizaciones.
   * Asignarle una calificación del `1` al `10`.
   * Escribir o reemplazar una reseña personal de hasta `750` caracteres.
   * Guardar automáticamente los cambios de vuelta en el archivo CSV original.
4. **Mostrar películas vistas:**
   * **Top k de películas:** Muestra el ranking de las mejores películas calificadas de mayor a menor (desempatando por cantidad de veces vista) mediante un **Max-Heap**.
   * **Listado completo:** Recorre y muestra todas las películas marcadas como vistas.
   * **Consulta individual:** Muestra la ficha detallada, calificación y reseña de una película vista.
  
## Arquitectura y Estructuras de Datos

El proyecto implementa Tipos de Datos Abstractos (TDAs) modulares ubicados en la carpeta `proyecto_c/tdas/`:

| Estructura | Implementación Física | Uso en el Proyecto |
| :--- | :--- | :--- |
| **Mapa de Películas** | Tabla Hash con encadenamiento | Acceso directo por título en O(1) esperado. |
| **Mapa de Años** | Multimapa (Tabla Hash + Listas) | Agrupación y búsqueda de películas por año de lanzamiento. |
| **Lista de Ingreso** | Lista doblemente enlazada | Conserva el orden original del CSV para recorridos y guardado. |
| **Heap de Ranking** | Max-Heap sobre arreglo dinámico | Extracción eficiente de las k películas mejor calificadas en O(k \log C). |

## Dentro de la propia carpeta "proyecto_c" hay un README que detalla el uso de la aplicación 
