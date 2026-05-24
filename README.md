# Sistema de Sucesión Real con Árboles Binarios

Implementación en C++ de un sistema de gestión de linaje real basado en árboles binarios, desarrollado **sin librerías de terceros**. El programa carga un linaje desde un archivo CSV, muestra la línea de sucesión y calcula automáticamente quién debe heredar el trono aplicando reglas de primogenitura, edad y género.

Proyecto desarrollado para la asignatura *Estructuras Dinámicas de Datos* (2025C).

## Características

- **Carga de datos** — lectura y parseo manual de archivos `.csv` para construir el árbol binario en memoria, sin usar librerías externas de parsing.
- **Visualización** — muestra la línea de sucesión actual mediante un recorrido recursivo en preorden.
- **Motor de sucesión** — algoritmo de búsqueda en profundidad (DFS) que determina al heredero aplicando estas prioridades:
  - Descendientes (rama izquierda) antes que colaterales (rama derecha).
  - Varones menores de 70 años.
  - Mujeres mayores de 15 años como alternativa, si no hay varón apto.
  - Retiro forzoso del rey al cumplir 71 años o al morir.
- **Edición en tiempo de ejecución** — modificación de los atributos de cada noble (nombre, edad, estado vital).

## Diseño del árbol

Cada nodo (`RoyalNode`) representa a un miembro del linaje y usa los punteros del árbol binario con una semántica particular:

- **`left`** — primogénito (hijo mayor).
- **`right`** — siguiente hermano.
- **`parent`** — referencia hacia el ascendiente.

Esta representación *hijo-izquierdo / hermano-derecho* permite modelar un árbol genealógico n-ario usando únicamente un árbol binario. La búsqueda de nodos por ID se realiza de forma recursiva, sin estructuras auxiliares como `std::map`, cumpliendo el requisito de implementar las estructuras de datos manualmente.

## Stack tecnológico

- **C++** — únicamente con la biblioteca estándar (`<iostream>`, `<fstream>`, `<sstream>`, `<string>`).
- Sin dependencias ni librerías de terceros.

## Compilación y ejecución

Requiere un compilador de C++ (por ejemplo, `g++`).

```bash
# Compilar
g++ src/main.cpp -o bin/main.exe

# Ejecutar
./bin/main.exe
```

Al iniciar, el programa carga `bin/royal_data.csv` y muestra un menú interactivo con las opciones: ver la línea de sucesión, asignar un nuevo rey, editar un noble o salir.

## Ejemplo de ejecución

```
Datos cargados exitosamente.

1. Ver Linea
2. Asignar Rey
3. Editar
4. Salir
Op: 1

--- Linea de Sucesion (Vivos) ---
- Terenas Menethil (71 a, H) [REY/REINA ACTUAL]
- Anduin Wrynn (22 a, H)
- Bolvar Fordragon (5 a, H)
- Jaina Proudmoore (21 a, M)
- Muradin Bronzebeard (50 a, H)
- Magni Bronzebeard (55 a, H)
- Moira Thaurissan (25 a, M)

1. Ver Linea
2. Asignar Rey
3. Editar
4. Salir
Op: 2
Reinado de Terenas terminado.
>>> NUEVO REY/REINA: Anduin <<<

1. Ver Linea
2. Asignar Rey
3. Editar
4. Salir
Op: 1

--- Linea de Sucesion (Vivos) ---
- Terenas Menethil (71 a, H)
- Anduin Wrynn (22 a, H) [REY/REINA ACTUAL]
- Bolvar Fordragon (5 a, H)
- Jaina Proudmoore (21 a, M)
- Muradin Bronzebeard (50 a, H)
- Magni Bronzebeard (55 a, H)
- Moira Thaurissan (25 a, M)
```

En el ejemplo, el rey Terenas tiene 71 años, por lo que al asignar un nuevo rey el sistema lo retira del trono y recorre el árbol en profundidad hasta encontrar al primer heredero apto: Anduin, varón menor de 70 años de la rama de descendientes.

## Estructura del proyecto

```
.
├── README.md
├── src/
│   └── main.cpp        # Código fuente (estructura RoyalNode y clase RoyalTree)
└── bin/
    ├── main.exe        # Ejecutable (generado tras compilar)
    └── royal_data.csv  # Datos iniciales del linaje
```

## Formato del archivo CSV

El archivo `royal_data.csv` define el linaje con las siguientes columnas:

```
id, name, last_name, gender, age, id_father, is_dead, was_king, is_king
```

Donde `gender` es `H` (hombre) o `M` (mujer), e `id_father` enlaza cada noble con su ascendiente para reconstruir el árbol.

## Autor

**Luis Fernando Chunwa Chow Cheung**
Estudiante de Ingeniería en Computación — Universidad Rafael Urdaneta
GitHub: [@LuisChow](https://github.com/LuisChow)
