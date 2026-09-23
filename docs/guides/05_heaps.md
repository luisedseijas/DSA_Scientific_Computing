# Guía 05: Heaps y Colas de Prioridad (`dsa::BinaryHeap<T, Compare>`, `dsa::PriorityQueue<T, Compare>`)

## 1. Motivación: ¿cuál es el máximo, rápido, todo el tiempo?

Supongamos que necesitamos una estructura que soporte, repetidamente:

1. Insertar un elemento nuevo.
2. Preguntar cuál es el elemento "más importante" (el máximo, o el mínimo,
   según cómo definamos "importante").
3. Extraer (quitar y devolver) ese elemento más importante.

Un arreglo ordenado nos daría extracción del máximo en O(1) (está al final),
pero insertar en la posición correcta cuesta O(n) por el desplazamiento. Un
arreglo sin ordenar nos daría inserción en O(1), pero encontrar el máximo
cuesta O(n). Un árbol binario de búsqueda balanceado nos daría O(log n) para
ambas operaciones, pero es una estructura con más invariantes de las que
realmente necesitamos aquí: no nos importa poder buscar un elemento
arbitrario ni recorrer todo en orden, solo repetidamente sacar el máximo.

El **heap binario** resuelve exactamente este problema, con inserción y
extracción del máximo (o mínimo) en O(log n), usando nada más que un
arreglo. Es la estructura de datos detrás de las **colas de prioridad**
(`PriorityQueue`), de **heap sort**, y —como veremos en el módulo de
grafos— del algoritmo de Dijkstra para caminos mínimos.

## 2. La forma: árbol binario casi completo

Un heap es conceptualmente un **árbol binario casi completo** (*complete
binary tree*): todos los niveles están completamente llenos excepto,
posiblemente, el último, que se llena **de izquierda a derecha** sin dejar
huecos.

```
                    (nivel 0)        9
                                   /   \
                    (nivel 1)    7       8
                                /  \    /  \
                    (nivel 2) 3    5  6    2
                              /\
                    (nivel 3)1  4
```

Esta forma tan restringida (a diferencia de un árbol binario cualquiera) es
justo lo que nos permite representarlo **sin punteros**, en un arreglo
plano, indexando de izquierda a derecha, nivel por nivel:

```
índice:   0  1  2  3  4  5  6  7  8
valor:    9  7  8  3  5  6  2  1  4
```

### 2.1 Fórmulas de índice

Para un nodo almacenado en la posición `i` del arreglo (0-indexado):

| Relación         | Fórmula          |
|------------------|-------------------|
| Padre de `i`     | `(i - 1) / 2` (división entera) |
| Hijo izquierdo   | `2*i + 1`         |
| Hijo derecho     | `2*i + 2`         |

Estas fórmulas son la razón de ser del heap binario: reemplazan los
punteros `left`/`right`/`parent` de un árbol enlazado por aritmética pura
sobre un índice, así que no hay asignaciones de memoria por nodo ni
overhead de punteros. Es exactamente lo mismo que `dsa::BinaryHeap` usa en
`parent_of`, `left_of` y `right_of` (ver
`include/dsa/trees/binary_heap.hpp`).

Como el árbol es casi completo, su altura es siempre `O(log n)`: cada nivel
tiene el doble de nodos que el anterior, así que con `n` nodos hay
`⌊log₂ n⌋ + 1` niveles. Esta altura logarítmica es la que le da a las
operaciones del heap su complejidad O(log n).

## 3. La invariante de heap (heap property)

Un heap **no** está totalmente ordenado como un BST: solo garantiza una
relación de orden entre cada nodo y sus hijos directos, no entre "hermanos"
ni "primos".

- **Max-heap**: para todo nodo `i` (distinto de la raíz), `valor(padre(i)) >=
  valor(i)`. La raíz (índice 0) es siempre el elemento máximo.
- **Min-heap**: la desigualdad se invierte; la raíz es siempre el mínimo.

`dsa::BinaryHeap<T, Compare>` generaliza esto con un comparador, igual que
`std::priority_queue`: con `Compare = std::less<T>` (el valor por defecto)
se obtiene un max-heap; pasando `std::greater<T>` se obtiene un min-heap.
La condición interna que se usa (`sift_up`/`sift_down`) es "¿`Compare(a, b)`
es verdadero?", que se lee como "`a` debería quedar por debajo de `b` en el
heap".

Nótese que la invariante **no dice nada** sobre el orden entre hijos
izquierdo y derecho, ni entre elementos de ramas distintas. Por eso un heap
no sirve para búsqueda binaria ni para recorrerlo "en orden": solo garantiza
acceso rápido al extremo (máximo o mínimo).

## 4. `sift_up`: restaurar la invariante tras insertar

Insertar un elemento nuevo es sencillo en cuanto a *forma*: se coloca en la
siguiente posición libre del arreglo (el final), lo cual mantiene el árbol
completo automáticamente. El problema es que ese elemento nuevo puede
romper la invariante de heap con su padre.

`sift_up` (también llamado *bubble up* o *percolate up*) resuelve esto:
mientras el elemento sea "mejor" que su padre según `Compare`, se
intercambia con él, subiendo un nivel. El proceso se detiene en cuanto
encuentra un padre que ya lo domina, o al llegar a la raíz.

```
insert(10) en el heap de arriba:

    9                    9                    10
  /   \                /   \                /    \
 7     8      ->      7     8      ->      7       8
/ \   / \            / \   / \  \          / \     / \  \
3  5 6  2           3  5 6  2  10         3   5   6   2  9
/\                  /\                    /\
1 4                 1 4                   1  4
```

Como el elemento solo puede subir por **un único camino** desde una hoja
hasta la raíz, y la altura del árbol es O(log n), `sift_up` hace a lo sumo
O(log n) comparaciones e intercambios.

## 5. `sift_down`: restaurar la invariante tras extraer

Extraer el elemento de la raíz (el máximo o mínimo) también tiene un
problema de forma: no podemos simplemente dejar un hueco en el índice 0, ni
podemos quitar cualquier otro nodo sin romper la propiedad de "árbol
completo".

La solución clásica: mover el **último** elemento del arreglo (la última
hoja) a la posición de la raíz —esto preserva la forma completa, porque
solo quitamos el último slot del arreglo— y luego dejar que ese elemento
"se hunda" hasta encontrar su lugar correcto.

`sift_down` (también *bubble down*, *percolate down*, o *heapify* de un
solo nodo) compara el nodo con sus dos hijos, y si alguno de ellos debería
estar por encima según `Compare`, se intercambia con el **mejor** de los
dos hijos (el máximo de los dos, en un max-heap), y se repite el proceso
desde la nueva posición. Se detiene cuando el nodo ya domina a ambos hijos,
o al llegar a una hoja.

Igual que `sift_up`, esto recorre a lo sumo un camino de raíz a hoja, así
que también es O(log n).

## 6. `top()` / `peek()`: O(1)

Como la invariante garantiza que el extremo siempre está en el índice 0,
consultarlo sin extraerlo es simplemente `data_[0]`: O(1).

## 7. `build_heap`: por qué es O(n) y no O(n log n)

Si quisiéramos construir un heap a partir de `n` elementos desordenados,
la forma ingenua sería insertarlos uno por uno con `insert` (que hace
`sift_up`, O(log n) cada uno), dando un total de O(n log n).

Existe un algoritmo mejor, conocido como **heapify de Floyd**: copiar los
`n` elementos al arreglo tal cual están (sin ningún orden), y luego aplicar
`sift_down` a cada nodo **interno** (no-hoja), empezando por el último nodo
interno y retrocediendo hasta la raíz (índice 0). Esto es lo que hace
`BinaryHeap::build_heap()`.

¿Por qué empezar desde el último nodo interno? Porque toda hoja es, por sí
sola, un heap válido de un solo elemento (no necesita hundirse). Al
procesar los nodos de abajo hacia arriba, cuando le toca el turno a un nodo,
sus subárboles ya son heaps válidos, así que `sift_down` sobre él es
suficiente para arreglar todo el subárbol enraizado ahí.

### 7.1 El argumento de la suma geométrica

La clave de por qué esto es O(n) —y no O(n log n)— es que **la mayoría de
los nodos de un árbol casi completo están cerca de las hojas**, donde
`sift_down` hace muy poco trabajo.

En un árbol casi completo con `n` nodos y altura `h ≈ log₂ n`:

- En el nivel más bajo (las hojas) hay aproximadamente `n/2` nodos, y cada
  uno requiere 0 pasos de `sift_down` (son heaps triviales; de hecho ni
  siquiera se procesan).
- Un nivel por encima hay aproximadamente `n/4` nodos, cada uno a lo sumo a
  1 paso de distancia de una hoja.
- Dos niveles por encima, `n/8` nodos, a lo sumo 2 pasos.
- ... en general, en el nivel a distancia `d` de las hojas hay
  aproximadamente `n / 2^(d+1)` nodos, cada uno haciendo a lo sumo `d`
  pasos de `sift_down`.

El costo total está acotado por:

```
T(n) = Σ_{d=0}^{log n} (n / 2^(d+1)) * d
     = n * Σ_{d=0}^{∞} d / 2^(d+1)
```

La serie `Σ d / 2^(d+1)` para `d = 0, 1, 2, ...` **converge a una
constante** (se puede demostrar que converge a 1, usando el mismo tipo de
manipulación algebraica que la serie geométrica estándar `Σ x^d = 1/(1-x)`,
derivada respecto de `x`). Por lo tanto:

```
T(n) = n * O(1) = O(n)
```

Esto es un ejemplo célebre de por qué "n operaciones de O(log n) cada una"
**no siempre** da O(n log n): la cota de O(log n) por operación es correcta
pero muy pesimista para la mayoría de los nodos, que están cerca de las
hojas y hacen mucho menos trabajo. `insert` uno por uno no tiene esta
ventaja porque **cada** inserción nueva entra por una hoja y potencialmente
sube hasta la raíz (O(log n) en el peor caso, y en promedio también es
costoso porque el árbol ya está "cargado" con elementos grandes cerca de la
raíz).

## 8. Tabla de complejidad

| Operación                          | Complejidad  |
|-------------------------------------|--------------|
| `top()` / `peek()`                  | O(1)         |
| `insert()` / `push()`               | O(log n) amortizado (O(1) amortizado + O(log n) `sift_up`) |
| `extract_top()` / `pop()`           | O(log n)     |
| `build_heap()` desde `n` elementos  | O(n)         |
| `empty()`, `size()`                 | O(1)         |
| `clear()`                           | O(n)         |
| Espacio                             | O(n)         |

(El "amortizado" en `insert` viene del mismo crecimiento geométrico del
buffer que usa `DynamicArray`; ver la Guía 01 para el análisis completo de
por qué duplicar la capacidad da O(1) amortizado por el redimensionamiento
en sí, independientemente del costo de `sift_up`.)

## 9. `PriorityQueue`: un adaptador, no una reimplementación

`dsa::PriorityQueue<T, Compare>` (en
`include/dsa/trees/priority_queue.hpp`) **no** reimplementa ningún
algoritmo de heap: es un envoltorio delgado (*thin wrapper*) alrededor de
`BinaryHeap<T, Compare>` que solo expone el vocabulario clásico de cola de
prioridad (`push`, `pop`, `top`, `empty`, `size`). Esto es intencional y
refleja cómo `std::priority_queue` de la biblioteca estándar también es un
*adaptador de contenedor* sobre otra estructura (típicamente
`std::vector`) en vez de una implementación independiente. La lección de
diseño: cuando dos ADTs comparten exactamente la misma estructura interna,
tiene sentido expresar uno en términos del otro en vez de duplicar la
lógica.

## 10. Aplicaciones

### 10.1 Heap sort

Si se construye un heap con `build_heap` (O(n)) y luego se extrae el
elemento tope repetidamente hasta vaciarlo (`n` extracciones de O(log n)
cada una), el resultado es una secuencia completamente ordenada. Esto da un
algoritmo de ordenamiento de **O(n log n)** en el peor caso, in-place (si
se implementa sobre el mismo arreglo de entrada, sin usar un heap
"externo"), y sin el peor caso cuadrático de quicksort. Es la base de
**heap sort**.

### 10.2 Colas de prioridad

El uso más directo: cualquier sistema que deba procesar tareas por
prioridad en vez de por orden de llegada (planificadores de sistemas
operativos, simulación de eventos discretos, compresión de Huffman) usa
una cola de prioridad respaldada por un heap.

### 10.3 Adelanto: el algoritmo de Dijkstra

En el módulo de grafos verán el algoritmo de Dijkstra para caminos mínimos
de fuente única. Su implementación eficiente depende de una cola de
prioridad (min-heap) para siempre expandir a continuación el nodo con la
menor distancia tentativa conocida — exactamente la operación
`extract_top()` que ya tienen aquí, con `Compare = std::greater<T>`. Sin un
heap, Dijkstra tendría que buscar linealmente el mínimo en cada paso
(O(V²) en total); con un heap binario, baja a O((V + E) log V).

## 11. Ejercicios de extensión

1. **`heap_sort`**: implementen una función libre
   `template <typename T, typename Compare = std::less<T>> void heap_sort(T* data, std::size_t n)`
   que use `dsa::BinaryHeap<T, Compare>` (con `build_heap` sobre el rango
   `[data, data + n)` y extracciones sucesivas) para dejar `data` ordenado.
   Piensen: ¿con qué `Compare` deben construir el heap si quieren orden
   ascendente al final? (Pista: no es el mismo `Compare` que usarían para
   una cola de prioridad de "máxima prioridad primero".)

2. **K-ésimo elemento más grande con heap de tamaño k**: dado un flujo
   (stream) de números que llegan uno a la vez, mantengan el k-ésimo mayor
   visto hasta el momento usando un **min-heap de tamaño acotado a k**: al
   insertar un nuevo elemento, si el heap tiene menos de `k` elementos,
   insértenlo; si ya tiene `k` y el nuevo elemento es mayor que el mínimo
   actual (`top()` del min-heap), extraigan el mínimo e inserten el nuevo.
   Al final, `top()` es el k-ésimo elemento más grande visto. Analicen la
   complejidad por elemento procesado (debería ser O(log k), no O(log n)).

3. **Fusionar dos heaps**: dado `BinaryHeap<T, Compare> a` y
   `BinaryHeap<T, Compare> b`, escriban una función que produzca un heap
   que contenga todos los elementos de ambos. La solución ingenua (extraer
   todo de `b` e insertarlo uno por uno en `a`) da O(m log(n + m)) donde
   `m = b.size()`. ¿Pueden hacerlo en O(n + m) usando la idea de
   `build_heap`? (Pista: no necesitan preservar el orden relativo de
   inserción; solo necesitan que el resultado final sea un heap válido.)

4. *(Opcional, más avanzado)* Investiguen por qué un heap binario **no**
   soporta `decrease_key` (bajar la prioridad de un elemento arbitrario ya
   insertado) en menos de O(n) en general —no hay forma directa de ubicar
   ese elemento en el arreglo sin buscarlo linealmente— y por qué esto
   motiva estructuras más sofisticadas como el *heap de Fibonacci*, usado
   en implementaciones optimizadas de Dijkstra y Prim.
