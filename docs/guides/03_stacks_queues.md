# Guía 03: Pilas, Colas y Deques

## 1. Introducción: dos disciplinas de acceso

Una **pila** (`Stack`) y una **cola** (`Queue`) son estructuras lineales que
restringen *dónde* se puede insertar y eliminar un elemento. A diferencia de
`DynamicArray`, que permite acceso aleatorio a cualquier posición, estas
estructuras solo exponen los extremos. Esa restricción no es una limitación
arbitraria: es lo que las hace predecibles, eficientes y fáciles de razonar.

- **Pila (Stack): disciplina LIFO** (*Last In, First Out* — el último en
  entrar es el primero en salir). Todo ocurre en un único extremo, llamado
  el **tope** (*top*). `push` agrega al tope, `pop` remueve del tope.
- **Cola (Queue): disciplina FIFO** (*First In, First Out* — el primero en
  entrar es el primero en salir). Hay dos extremos: el **frente** (*front*),
  por donde se remueve, y el **fondo** (*back*), por donde se inserta.
  `enqueue` agrega al fondo, `dequeue` remueve del frente.
- **Deque** (*double-ended queue*, "deck"): generaliza ambas. Permite
  insertar y remover en **ambos** extremos: `push_front`, `push_back`,
  `pop_front`, `pop_back`. Una pila y una cola pueden implementarse *encima*
  de un deque usando solo uno o dos de sus cuatro métodos.

En este curso, `Stack`, `Queue` y `Deque` se implementan sobre un buffer
propio (`new`/`delete`), heredando de `Collection<T>`, siguiendo el mismo
espíritu que `DynamicArray`.

## 2. Pila: un arreglo dinámico visto desde un solo extremo

Una pila implementada con arreglo dinámico es casi idéntica a
`DynamicArray`: el tope de la pila es simplemente el último elemento del
arreglo (`data_[size_ - 1]`). `push` es `push_back`, `pop` es `pop_back`.
Como ambas operaciones tocan el mismo extremo, no hay nada que desplazar:
son O(1) amortizado, con crecimiento geométrico (duplicar capacidad) cuando
el buffer se llena.

## 3. Cola: por qué un arreglo simple es una mala idea

Imaginemos una cola ingenua construida sobre un arreglo simple, donde el
frente siempre vive en el índice 0:

```
enqueue(a): [a]
enqueue(b): [a, b]
dequeue():  [b]        <- hay que mover b desde el índice 1 al 0
enqueue(c): [b, c]
dequeue():  [c]        <- otra vez, desplazar todo
```

Cada `dequeue` obliga a desplazar **todos** los elementos restantes una
posición a la izquierda para mantener el frente en el índice 0. Eso es
O(n) por cada `dequeue`, lo cual es inaceptable para una estructura que se
usa, por ejemplo, en cada iteración de un recorrido BFS sobre un grafo
grande.

### La solución: buffer circular (ring buffer)

En vez de mover elementos, movemos un **índice**. Mantenemos:

- `front_`: índice físico del primer elemento lógico.
- `size_`: cuántos elementos hay actualmente.
- `capacity_`: tamaño del buffer.

El elemento lógico en la posición `i` (contando desde el frente) vive en el
índice físico `(front_ + i) % capacity_`. Al hacer `dequeue`, simplemente
avanzamos `front_` una posición (módulo `capacity_`) sin tocar el resto del
buffer. Al hacer `enqueue`, insertamos en `(front_ + size_) % capacity_` y
aumentamos `size_`.

```
capacidad = 4, buffer: [_, _, _, _]
enqueue(a): front=0 size=1  [a, _, _, _]
enqueue(b): front=0 size=2  [a, b, _, _]
dequeue():  front=1 size=1  [a, b, _, _]   <- 'a' sigue en memoria pero es basura lógica
enqueue(c): front=1 size=2  [a, b, c, _]
enqueue(d): front=1 size=3  [a, b, c, d]
enqueue(e): front=1 size=4  [e, b, c, d]   <- 'e' "da la vuelta" (wraparound) al índice 0
```

Cuando el índice llega al final del buffer, el operador módulo lo regresa al
inicio: el buffer se comporta como un círculo, de ahí el nombre. Esto hace
que **tanto `enqueue` como `dequeue` sean O(1)** de verdad (no amortizado),
sin desplazar ningún elemento. El crecimiento del buffer (cuando `size_ ==
capacity_`) sigue siendo geométrico y amortizado O(1), igual que en
`DynamicArray`, salvo que al copiar al nuevo buffer hay que "desenrollar" el
wraparound (copiar en orden lógico empezando en el índice 0 del nuevo
buffer).

## 4. Deque: circular en ambas direcciones

Un `Deque` usa exactamente el mismo esquema, pero `front_` también se puede
mover **hacia atrás**. `push_front` retrocede `front_` una posición (con
wraparound hacia el final del buffer si `front_` está en 0) y coloca el
elemento ahí; `push_back` funciona igual que en `Queue`. Así, los cuatro
extremos de inserción/eliminación son O(1).

## 5. Tabla de complejidad (Big-O)

| Operación                         | Stack | Queue | Deque |
|------------------------------------|:-----:|:-----:|:-----:|
| push / push_back                   | O(1)* | O(1)* | O(1)* |
| push_front                         |   —   |   —   | O(1)* |
| pop / pop_back                     | O(1)  | O(1)  | O(1)  |
| pop_front (dequeue)                |   —   | O(1)  | O(1)  |
| top / front / back                 | O(1)  | O(1)  | O(1)  |
| size / empty                       | O(1)  | O(1)  | O(1)  |
| clear                              | O(n)  | O(n)  | O(n)  |
| acceso aleatorio (índice arbitrario)| O(n)* | O(n)* | O(n)* |

\* O(1) amortizado: la mayoría de las llamadas son O(1) puro, pero cada
cierto número de operaciones el buffer se duplica, lo que cuesta O(n) esa
única vez. Promediado sobre muchas operaciones, el costo por operación
sigue siendo O(1). `clear` es O(n) porque hay que invocar el destructor de
cada elemento vivo, aunque la memoria del buffer no se libera. Ninguna de
estas tres estructuras expone acceso por índice en su API pública; se
incluye en la tabla solo como referencia de por qué no reemplazan a
`DynamicArray`.

## 6. Casos de uso reales

- **Pila — evaluación de expresiones y backtracking.** El algoritmo de
  Dijkstra del shunting-yard, la evaluación de expresiones en notación
  postfija (RPN), el chequeo de paréntesis balanceados, y la pila de
  llamadas de cualquier recursión (incluida la que usa el propio programa en
  ejecución) son pilas. En backtracking (por ejemplo, resolver un laberinto
  o un Sudoku), la pila guarda el camino actual: al llegar a un callejón sin
  salida, se hace `pop` para "deshacer" la última decisión y probar otra.
- **Cola — BFS (recorrido en anchura).** Al explorar un grafo o árbol nivel
  por nivel, se encolan los vecinos de un nodo al visitarlo y se procesan en
  el orden en que fueron descubiertos (FIFO). Esto garantiza que BFS
  encuentre el camino más corto en número de aristas. También se usan colas
  para simular sistemas de espera (impresoras, tareas en un scheduler,
  peticiones a un servidor).
- **Deque — ventanas deslizantes y trabajo en ambos extremos.** Algoritmos
  como el máximo/mínimo de una ventana deslizante mantienen un deque de
  índices donde se puede insertar/eliminar tanto por el frente como por el
  fondo según se necesite descartar candidatos obsoletos o antiguos. También
  se usa para implementar un `undo`/`redo` combinado, o para recorrer un
  buffer en ambas direcciones (por ejemplo, un algoritmo que procesa desde
  los dos extremos de un arreglo hacia el centro).

## 7. Ejercicios de extensión

1. **`getMin()` en O(1) para una pila.** Extiende (o compón) `Stack<T>` para
   soportar una operación `getMin()` que devuelva el valor mínimo actual en
   la pila en tiempo O(1), sin recorrer todos los elementos. Pista: mantén
   una segunda pila auxiliar que solo guarda el mínimo "vigente" en cada
   momento, y sincronízala en cada `push`/`pop`.

2. **Cola implementada con dos pilas.** Implementa una cola usando
   únicamente dos objetos `Stack<T>` (sin buffer circular propio).
   `enqueue` inserta en una pila "de entrada"; `dequeue` debe extraer de una
   pila "de salida", transfiriendo elementos de la pila de entrada a la de
   salida solo cuando la de salida está vacía. Analiza por qué el costo
   amortizado de `dequeue` sigue siendo O(1) aunque una operación individual
   pueda costar O(n).

3. **Máximo de una ventana deslizante con `Deque`.** Dado un arreglo de `n`
   enteros y un tamaño de ventana `k`, calcula el máximo de cada ventana de
   tamaño `k` que se desliza de izquierda a derecha, en tiempo total O(n).
   Pista: mantén en el deque los *índices* del arreglo en orden decreciente
   de sus valores; al avanzar la ventana, elimina por el fondo los índices
   cuyo valor es menor que el del elemento entrante (ya no pueden ser el
   máximo de ninguna ventana futura), y elimina por el frente los índices
   que quedaron fuera de la ventana actual.

4. **(Opcional, más difícil) Pila que soporta `getMax()` y `getMin()`
   simultáneamente**, o una cola con prioridad mínima construida sobre un
   `Deque` monotónico, análogo al ejercicio 3 pero soportando inserciones y
   eliminaciones por ambos extremos.
