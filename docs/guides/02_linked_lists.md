# Guía 02: Listas Enlazadas

## 1. Nodos y punteros: la idea central

Un `DynamicArray` (véase `linear/dynamic_array.hpp`) guarda sus elementos en un
bloque contiguo de memoria: acceder al elemento `i` es aritmética de punteros
(`base + i`). Una **lista enlazada** abandona esa contigüidad. En su lugar,
cada elemento vive en un **nodo** independiente, reservado dinámicamente, que
contiene:

1. El dato (`T data`).
2. Uno o más **punteros** hacia otros nodos de la estructura.

```
struct Node {
    T data;
    Node* next;   // (y opcionalmente Node* prev)
};
```

La lista en sí no almacena los elementos: solo guarda un puntero al primer
nodo (`head`) y, normalmente, al último (`tail`). Recorrer la lista consiste
en seguir la cadena de punteros `next` desde `head` hasta encontrar
`nullptr` (o, en el caso circular, hasta volver al punto de partida).

Esta indirección tiene dos consecuencias fundamentales:

- **Insertar y eliminar en un punto conocido de la lista es O(1)**: solo hay
  que reasignar un puñado de punteros, sin desplazar el resto de los
  elementos (a diferencia de un arreglo dinámico, donde insertar al inicio
  implica mover todos los elementos posteriores).
- **El acceso aleatorio (`list[i]`) es O(n)**: no hay aritmética de
  punteros posible; hay que caminar nodo a nodo desde el principio (o, en
  listas doblemente enlazadas, desde el extremo más cercano).

## 2. Simple, doble y circular: diferencias

### 2.1 Lista simplemente enlazada (`SinglyLinkedList<T>`)

Cada nodo apunta solo a su sucesor. Es la estructura más económica en
memoria (un puntero por nodo), pero solo se puede recorrer hacia adelante.

```
head ──▶[ 1 | •─┼──▶[ 2 | •─┼──▶[ 3 | •─┼──▶ nullptr
tail ────────────────────────────────▲
                                (apunta aquí)
```

Mantener un puntero `tail` permite `push_back` en O(1), pero `pop_back`
sigue siendo O(n): para saber cuál es el "penúltimo" nodo (el nuevo tail)
hay que recorrer la lista desde `head`, porque ningún nodo sabe quién es su
predecesor.

### 2.2 Lista doblemente enlazada (`DoublyLinkedList<T>`)

Cada nodo guarda un puntero `next` y otro `prev`. El costo es el doble de
memoria por puntero, pero a cambio:

- `pop_back` es O(1) (el nodo tail conoce a su predecesor directamente).
- Se puede recorrer en ambas direcciones (`begin()/end()` y
  `rbegin()/rend()`).
- Eliminar un nodo dado (si ya se tiene un iterador a él) es O(1), sin
  necesidad de buscar su predecesor.

```
nullptr ◀─┼[ 1 |•|•]⇄[ 2 |•|•]⇄[ 3 |•|•]─┼▶ nullptr
          ▲head                    tail▲
```

### 2.3 Lista circular (`CircularLinkedList<T>`)

Variante simplemente enlazada donde el último nodo, en vez de apuntar a
`nullptr`, apunta de nuevo al primero. No existe un "final" natural.

```
        ┌─────────────────────────────┐
        ▼                             │
head ──▶[ 1 |•]──▶[ 2 |•]──▶[ 3 |•]────┘
                              ▲
                             tail
```

Esta ausencia de `nullptr` es la razón por la que su iterador no puede
comparar contra un centinela de puntero: en `CircularLinkedList::Iterator`
se cuenta cuántos pasos faltan (`steps_remaining_`) y se detiene tras
visitar `size()` nodos, evitando así un bucle infinito. Guardar únicamente
`tail` (con `tail->next` como `head`) basta para tener acceso O(1) a ambos
extremos con un solo puntero miembro.

La operación característica de esta estructura es `rotate(n)`: desplaza
lógicamente cuál nodo se considera "cabeza", simplemente avanzando (o
retrocediendo) el puntero `tail` — sin mover un solo elemento de sitio. Es
la base de estructuras como buffers circulares o de la calendarización
*round-robin*.

## 3. Complejidad: listas enlazadas vs. `DynamicArray`

| Operación                     | DynamicArray | SinglyLinkedList | DoublyLinkedList | CircularLinkedList |
|--------------------------------|:---:|:---:|:---:|:---:|
| Acceso por índice `[i]`        | O(1) | O(n) | O(n) | O(n) |
| Búsqueda por valor              | O(n) | O(n) | O(n) | O(n) |
| Insertar/eliminar al inicio      | O(n) | O(1) | O(1) | O(1) |
| Insertar/eliminar al final       | O(1) amortizado | O(1)* | O(1) | O(1) |
| Insertar/eliminar en medio (con iterador) | O(n) | O(n)** | O(1) | O(n)** |
| Memoria extra por elemento       | ninguna | 1 puntero | 2 punteros | 1 puntero |
| Localidad de caché               | excelente | pobre | pobre | pobre |

\* `push_back` es O(1) gracias al puntero `tail`; `pop_back` es O(n) porque
hay que recorrer la lista para encontrar el nuevo tail.
\*\* Insertar/eliminar en medio requiere O(n) para *llegar* al nodo (buscar
la posición), aunque la operación de reenlace en sí sea O(1) una vez
ubicado.

**Conclusión práctica:** si el acceso es mayormente secuencial y se
insertan/eliminan elementos en los extremos o cerca de una posición ya
conocida, una lista enlazada es preferible. Si se necesita acceso aleatorio
frecuente o buena localidad de caché (rendimiento real en hardware
moderno), `DynamicArray` suele ganar a pesar de su complejidad teórica peor
en inserciones al inicio.

## 4. Ejercicios de extensión

1. **Lista circular doblemente enlazada.** Combina las ideas de
   `DoublyLinkedList` y `CircularLinkedList`: cada nodo tiene `prev` y
   `next`, y el último nodo se conecta con el primero en ambas direcciones.
   Implementa `push_front`, `push_back`, `pop_front`, `pop_back` y
   `rotate(n)`, todos en O(1). ¿Qué ventaja tiene sobre la versión
   simplemente enlazada al rotar en sentido negativo?

2. **Detección de ciclos (algoritmo de Floyd, "tortuga y liebre").**
   Implementa una función libre `bool has_cycle(const SinglyLinkedList<T>&)`
   usando dos punteros que avanzan a distinta velocidad (uno un nodo por
   iteración, otro dos). Como referencia, formaliza primero por qué
   funcionaría incluso si a `SinglyLinkedList` se le permitiera (mal uso)
   apuntar `tail->next` de vuelta a un nodo intermedio. Analiza la
   complejidad en tiempo (O(n)) y en espacio (O(1)).

3. **Inversión in-place.** Escribe un método `reverse()` para
   `SinglyLinkedList<T>` y otro para `DoublyLinkedList<T>` que inviertan el
   orden de los elementos sin reservar nodos nuevos ni usar una estructura
   auxiliar, actualizando correctamente `head_`/`tail_` (y todos los
   `prev`/`next` en el caso doble). ¿Cuántos punteros temporales necesitas
   como mínimo en cada caso?

4. *(Opcional, más difícil)* **Fusionar dos listas ordenadas.** Dadas dos
   `SinglyLinkedList<T>` ya ordenadas, escribe `merge_sorted` que las
   combine en una sola lista ordenada reutilizando los nodos existentes
   (sin copiar datos), en O(n + m).
