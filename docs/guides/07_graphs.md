# 07 — Grafos y algoritmos de grafos

## 1. Qué es un grafo

Un grafo `G = (V, E)` es un conjunto de vértices `V` (o nodos) conectados por
aristas `E`. Cada arista puede ser:

- **Dirigida** (u -> v, solo se puede "viajar" de u a v) o **no dirigida**
  (u -- v, se puede viajar en ambos sentidos).
- **Ponderada** (tiene un peso/costo asociado, p. ej. distancia, tiempo,
  dinero) o **no ponderada** (todas las aristas cuestan "lo mismo": 1).

En `dsa::Graph<Vertex>` ambas dimensiones son parámetros: el constructor
recibe `bool directed`, y cada arista recibe un peso `double` que por
defecto vale `1.0` (así un grafo "sin pesos" simplemente ignora ese
parámetro y sigue comportándose de forma razonable).

## 2. Representación: lista vs. matriz de adyacencia

Hay dos formas clásicas de representar un grafo en memoria.

### Lista de adyacencia (la que usa este módulo)

Para cada vértice `v` guardamos la lista de sus vecinos (y, si el grafo es
ponderado, el peso de cada arista):

```
0: [(1, 4.0), (2, 1.0)]
1: [(0, 4.0), (2, 2.0), (3, 1.0)]
2: [(0, 1.0), (1, 2.0), (3, 5.0)]
3: [(1, 1.0), (2, 5.0), (4, 3.0)]
4: [(3, 3.0)]
```

- **Memoria**: `O(V + E)`. Ideal para grafos *dispersos* (pocas aristas
  respecto al máximo posible `V²`), que es el caso típico en la práctica
  (redes de carreteras, redes sociales, dependencias de paquetes...).
- **Recorrer vecinos de v**: `O(deg(v))` — exactamente lo que se necesita.
- **Consultar si existe la arista (u, v)**: `O(deg(u))` en el peor caso, hay
  que recorrer la lista de u.

### Matriz de adyacencia (alternativa)

Una matriz `V x V` donde `M[u][v]` guarda el peso de la arista (o infinito /
un valor centinela si no existe):

- **Memoria**: `O(V²)` siempre, exista o no la arista. Para un grafo con
  10 000 vértices y pocas aristas, esto desperdicia muchísima memoria.
- **Consultar si existe la arista (u, v)**: `O(1)` — se gana rapidez aquí.
- **Recorrer todos los vecinos de v**: `O(V)`, porque hay que barrer la
  fila completa aunque casi todo sean "no existe".

**Regla práctica**: si el grafo es disperso (E ≈ V, como una red de
carreteras) usa lista de adyacencia. Si el grafo es denso (E ≈ V²) o
necesitas `has_edge` muy frecuente en un grafo pequeño, la matriz puede
ganar. La mayoría de aplicaciones reales son dispersas, por eso
`dsa::Graph` usa lista de adyacencia.

## 3. BFS vs. DFS

Ambos recorren todos los vértices alcanzables desde un origen en
`O(V + E)` tiempo, pero en orden distinto y con usos distintos.

### BFS (Breadth-First Search, recorrido en anchura)

Usa una **cola** (FIFO). Visita el origen, luego todos sus vecinos directos,
luego los vecinos de esos vecinos, etc. — nivel por nivel.

**Úsalo cuando**: necesitas el camino más corto en número de aristas (en
un grafo no ponderado), o quieres explorar "lo más cercano primero" (p. ej.
"¿cuál es el grado de separación mínimo entre dos personas en una red
social?").

### DFS (Depth-First Search, recorrido en profundidad)

Usa una **pila** (LIFO). Se mete lo más profundo posible por un camino
antes de retroceder ("backtrack") y probar otro.

**Úsalo cuando**: quieres explorar todo un componente sin importar el
orden (detección de ciclos, componentes conexas, orden topológico), o el
grafo es muy profundo y no te interesa el camino más corto.

`dsa::dfs` está implementado de forma **iterativa** con una pila explícita
(`std::vector<Vertex>` usado como pila), no con recursión. La razón es
práctica: una implementación recursiva usa una llamada de función por cada
vértice visitado, y en un grafo con un camino muy largo (miles de vértices
en cadena) eso puede agotar la pila de llamadas del programa (*stack
overflow*). La versión iterativa evita ese límite. Para comparación en
clase, `dsa::dfs_recursive` ofrece la versión recursiva clásica — nótese
que puede visitar los vecinos en un orden ligeramente distinto al de la
versión iterativa, porque esta última apila todos los vecinos antes de
desapilar ninguno; ambos son órdenes DFS válidos.

Un punto importante que los tests verifican explícitamente: si el grafo
tiene **componentes desconectadas**, BFS/DFS desde un vértice `v` **solo**
visitan el componente al que pertenece `v`. Los vértices de otras
componentes nunca aparecen en el resultado.

## 4. Dijkstra: caminos más cortos con pesos no negativos

### Idea

Dijkstra calcula la distancia mínima desde un vértice origen `s` a todos
los demás vértices alcanzables, en un grafo con pesos **no negativos**.

La idea es voraz (*greedy*): mantenemos una distancia tentativa a cada
vértice (inicialmente `0` para `s`, infinito para el resto) y una cola de
prioridad ordenada por esa distancia. Repetidamente extraemos el vértice
no procesado con menor distancia tentativa — como los pesos son no
negativos, esa distancia ya es definitiva — y *relajamos* sus aristas: si
`dist(u) + peso(u, v) < dist(v)`, actualizamos `dist(v)`.

### Ejemplo paso a paso

Grafo no dirigido (el mismo que usan los tests):

```
        4.0
   0 ————————— 1
   |  \        |  \
1.0|   \2.0    |1.0\
   |    \      |    \
   2 ————\———— +     3
       5.0\         /
            \      /3.0
             3————4? (no, ver aristas reales abajo)
```

Aristas: `(0,1,4)`, `(0,2,1)`, `(1,2,2)`, `(1,3,1)`, `(2,3,5)`, `(3,4,3)`.

Ejecución desde `0`:

| Paso | Se extrae | dist[0] | dist[1] | dist[2] | dist[3] | dist[4] |
|------|-----------|---------|---------|---------|---------|---------|
| 0    | —         | 0       | ∞       | ∞       | ∞       | ∞       |
| 1    | 0 (d=0)   | 0       | 4       | 1       | ∞       | ∞       |
| 2    | 2 (d=1)   | 0       | **3** (1+2 < 4) | 1 | 6 (1+5) | ∞ |
| 3    | 1 (d=3)   | 0       | 3       | 1       | **4** (3+1 < 6) | ∞ |
| 4    | 3 (d=4)   | 0       | 3       | 1       | 4       | **7** (4+3) |
| 5    | 4 (d=7)   | 0       | 3       | 1       | 4       | 7           |

Distancias finales: `{0:0, 1:3, 2:1, 3:4, 4:7}` — exactamente lo que
verifica `graph_algorithms_test.cpp`. Nótese cómo el camino real más corto
a `1` no es la arista directa `(0,1,4)` sino el rodeo `0 -> 2 -> 1`
(`1 + 2 = 3 < 4`).

### ¿Por qué falla con pesos negativos?

La corrección de Dijkstra depende de que, al extraer el vértice con menor
distancia tentativa de la cola, esa distancia ya sea la definitiva y nunca
pueda mejorar después. Esto es cierto solo si todos los pesos son `>= 0`:
cualquier camino alternativo que pase por vértices aún no procesados solo
puede *aumentar* la distancia, nunca disminuirla.

Con un peso negativo esa garantía se rompe: podría existir un camino más
largo en número de aristas pero de menor costo total, que pase
precisamente por un vértice que Dijkstra ya dio por "cerrado" (procesado
de forma definitiva) con una distancia peor. Como Dijkstra nunca vuelve a
revisar un vértice ya extraído, se queda con la distancia incorrecta.

`dsa::dijkstra` detecta cualquier peso negativo en el grafo antes de
ejecutar el algoritmo y lanza `std::invalid_argument`, en vez de devolver
un resultado silenciosamente incorrecto. Para grafos con pesos negativos
(pero sin ciclos negativos) el algoritmo correcto es **Bellman-Ford** (ver
ejercicio de extensión más abajo).

## 5. Orden topológico

Un **orden topológico** de un grafo dirigido acíclico (DAG) es un orden
lineal de los vértices tal que, para toda arista `u -> v`, `u` aparece
antes que `v`. Solo existe si el grafo no tiene ciclos (si A depende de B y
B depende de A, no hay ningún orden válido).

### Algoritmo de Kahn

`dsa::topological_sort` implementa el algoritmo de Kahn:

1. Calcula el grado de entrada (`in-degree`) de cada vértice: cuántas
   aristas entrantes tiene.
2. Mete en una cola todos los vértices con grado de entrada `0` (no
   dependen de nadie).
3. Repite: extrae un vértice `u` de la cola, agrégalo al resultado, y para
   cada vecino `v` de `u`, decrementa su grado de entrada; si llega a `0`,
   mételo en la cola.
4. Si al terminar el resultado no incluye todos los vértices, es porque
   quedaron vértices con grado de entrada `> 0` para siempre — eso solo
   puede pasar si hay un ciclo (se lanza `std::runtime_error`).

### Aplicaciones típicas

- **Compilación de código / build systems**: si el archivo `b.cpp` incluye
  `a.hpp`, `a` debe compilarse (o al menos procesarse) antes que `b`. Un
  ciclo de dependencias de compilación es un error de diseño.
- **Prerrequisitos de un plan de estudios**: si "Estructuras de Datos"
  requiere "Programación I", el orden topológico da una secuencia válida
  de cursos que respeta todos los prerrequisitos. Un ciclo (A requiere B,
  B requiere A) sería un plan de estudios imposible.
- **Gestores de paquetes** (npm, pip, apt): instalar dependencias en el
  orden correcto antes que el paquete que las necesita.
- **Hojas de cálculo**: recalcular celdas en el orden correcto cuando unas
  fórmulas dependen de otras.

## 6. Complejidad (Big-O)

Sea `V` el número de vértices y `E` el número de aristas.

| Algoritmo          | Tiempo              | Espacio extra | Notas |
|---------------------|---------------------|----------------|-------|
| `add_vertex`        | O(1) amortizado      | O(1)           | |
| `add_edge`           | O(1) amortizado      | O(1)           | |
| `remove_edge`        | O(deg(u))            | O(1)           | |
| `has_edge`           | O(deg(u))            | O(1)           | |
| `neighbors(v)`       | O(1)                  | O(1)           | Devuelve referencia, no copia |
| `bfs`                | O(V + E)              | O(V)           | |
| `dfs` / `dfs_recursive` | O(V + E)           | O(V)           | La recursiva además O(V) de pila de llamadas |
| `dijkstra`           | O((V + E) log V)      | O(V)           | Con `std::priority_queue` (heap binario) |
| `topological_sort`   | O(V + E)              | O(V)           | Algoritmo de Kahn |

## 7. Ejercicios de extensión

1. **Matriz de adyacencia**: implementa `dsa::AdjacencyMatrixGraph<Vertex>`
   como representación alternativa (usa `std::vector<std::vector<double>>`
   con un valor centinela, p. ej. infinito, para "no hay arista"). Compara
   experimentalmente el uso de memoria y el tiempo de `has_edge` contra
   `dsa::Graph` para un grafo disperso (V=10000, E≈20000) y uno denso
   (V=200, E≈15000).

2. **Detección de ciclos en grafo no dirigido con Union-Find**: implementa
   una función `bool has_cycle_undirected(const Graph<Vertex>&)` usando la
   estructura Union-Find (Disjoint Set Union): recorre las aristas una a
   una; si los dos extremos ya están en el mismo conjunto, hay un ciclo; si
   no, únelos. Compara esta técnica contra usar DFS con seguimiento del
   nodo padre para detectar el mismo tipo de ciclo.

3. **Bellman-Ford para pesos negativos**: implementa
   `std::unordered_map<Vertex, double> bellman_ford(const Graph<Vertex>&,
   Vertex source)` que funcione correctamente con pesos negativos (relaja
   todas las aristas `V - 1` veces) y que detecte y reporte (lanzando una
   excepción) la existencia de un **ciclo de peso negativo** alcanzable
   desde el origen, caso en el que no existe una distancia mínima bien
   definida.

4. *(Opcional, más avanzado)* **A\* (A-estrella)**: extiende Dijkstra para
   incorporar una heurística admisible `h(v)` que estime la distancia
   restante hasta un destino fijo, y compara cuántos vértices explora A*
   frente a Dijkstra puro para llegar al mismo destino en un grafo tipo
   cuadrícula (grid).
