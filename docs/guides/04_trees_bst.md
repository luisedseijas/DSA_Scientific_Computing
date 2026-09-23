# Guía 04: Árboles binarios y árboles binarios de búsqueda (BST)

Módulo relacionado: `include/dsa/trees/binary_tree.hpp` y
`include/dsa/trees/binary_search_tree.hpp`.

## 1. ¿Qué es un árbol binario?

Un **árbol binario** es una estructura jerárquica de nodos donde cada nodo
tiene como máximo dos hijos, llamados convencionalmente `left` (izquierdo) y
`right` (derecho). El nodo superior se llama **raíz**; un nodo sin hijos es
una **hoja**; la **altura** de un árbol es el número de aristas en el camino
más largo desde la raíz hasta una hoja (un árbol vacío tiene altura -1, un
árbol de un solo nodo tiene altura 0).

```
            50            <- raíz
          /    \
        30      70
       /  \    /  \
     20   40  60   80     <- hojas
```

`dsa::BinaryTree<T>` (en `binary_tree.hpp`) es la versión "sin orden": el
`insert` simplemente coloca cada nuevo elemento en la primera posición libre
recorriendo el árbol por niveles (level-order / BFS), igual que se hace al
construir un heap binario por posición. Esto da una forma concreta y
predecible (siempre un árbol "completo", relleno de izquierda a derecha)
antes de introducir la restricción de orden del BST.

## 2. El invariante del BST

`dsa::BinarySearchTree<T>` (en `binary_search_tree.hpp`) añade una regla de
orden: **para todo nodo `n`, todos los valores en el subárbol izquierdo son
menores que `n->value`, y todos los valores en el subárbol derecho son
mayores**. Esta propiedad debe cumplirse recursivamente en cada subárbol, no
solo en el nodo raíz.

Gracias a este invariante:

- El recorrido **in-order** siempre produce los elementos en orden
  ascendente (es la propiedad más importante y la que más se prueba con
  tests).
- `insert`, `find`/`contains` y `erase` pueden decidir en cada nodo "¿voy a
  la izquierda o a la derecha?" comparando con `operator<`, sin necesidad de
  recorrer todo el árbol.

En esta implementación, valores duplicados (que no son `<` ni `>`, es decir
son "iguales" según `operator<`) son ignorados por `insert`, igual que en
`std::set`.

## 3. `erase`: los tres casos

Eliminar un nodo de un BST manteniendo el invariante requiere distinguir
tres casos, según cuántos hijos tiene el nodo a borrar (`target`):

### Caso 1: `target` es una hoja (sin hijos)

Simplemente se desconecta del padre.

```
      50                     50
     /  \      erase(20)    /  \
   30    70   --------->  30    70
   /
  20
```

### Caso 2: `target` tiene un solo hijo

El hijo único ocupa el lugar de `target` (se "salta" el nodo eliminado).

```
      50                     50
     /  \      erase(30)    /  \
   30    70   --------->  40    70
     \
     40
```

### Caso 3: `target` tiene dos hijos

No se puede simplemente conectar ambos hijos al padre (el padre solo tiene
un hueco). La solución clásica: buscar el **sucesor in-order** de `target`
—el valor inmediatamente mayor, que es siempre el nodo más a la izquierda
del subárbol derecho de `target` (y por construcción no tiene hijo
izquierdo)— copiar (o, como en esta implementación, empalmar físicamente) ese
nodo al lugar de `target`, y luego eliminar el sucesor de su posición
original, que ahora es un caso 0 o 1 hijo.

```
        50                         50
      /    \      erase(30)      /    \
    30      70    --------->   40      70
   /  \    /  \                /  \    /  \
  20  40  60  80              20  (∅) 60   80
       \
      (nada, 40 no tiene left)
```

Aquí el sucesor de 30 es 40 (el nodo más a la izquierda del subárbol
derecho, que es solo `{40}`). 40 toma el lugar de 30 y hereda su hijo
izquierdo (20) y su hijo derecho (70's subárbol no cambia).

La implementación en `binary_search_tree.hpp` usa un helper `transplant`
(igual que en CLRS) para reconectar punteros `parent`/`left`/`right` de
forma uniforme en los tres casos.

## 4. Complejidad Big-O

| Operación         | Caso promedio (árbol balanceado) | Peor caso (árbol degenerado) |
|--------------------|:---------------------------------:|:------------------------------:|
| `insert`           | O(log n)                          | O(n)                            |
| `find`/`contains`  | O(log n)                          | O(n)                            |
| `erase`            | O(log n)                          | O(n)                            |
| `min` / `max`      | O(log n)                          | O(n)                            |
| `inorder` (completo) | O(n)                             | O(n)                            |

**¿Por qué O(log n) en promedio?** Cada comparación descarta (en promedio)
la mitad del árbol restante, igual que en búsqueda binaria sobre un arreglo
ordenado. Si el árbol tiene `n` nodos y está razonablemente balanceado, su
altura es aproximadamente `log2(n)`, y cada operación de búsqueda/inserción
recorre como mucho un camino raíz-hoja, es decir, `O(altura) = O(log n)`.

**¿Por qué O(n) en el peor caso?** Si los elementos se insertan en orden ya
ordenado (p. ej. 1, 2, 3, 4, 5, ...), cada nuevo nodo solo puede ir a la
derecha del anterior: el árbol **degenera** en una lista enlazada.

```
insertar 1,2,3,4,5 en ese orden:

1
 \
  2
   \
    3
     \
      4
       \
        5
```

Aquí la altura es `n - 1` en lugar de `log2(n)`, así que cualquier búsqueda
recorre potencialmente todos los nodos: O(n). El test
`BinarySearchTreeTest.HeightDegenerateTree` verifica exactamente este caso
(altura 4 para 5 nodos insertados en orden).

Esta es la motivación principal para estructuras auto-balanceadas como
AVL o árboles rojo-negro, que garantizan O(log n) en el peor caso
reestructurando el árbol tras cada inserción/eliminación (ver ejercicio 3).

## 5. Los cuatro recorridos, comparados

Dado el árbol:

```
            50
          /    \
        30      70
       /  \    /  \
     20   40  60   80
```

| Recorrido    | Orden de visita               | Resultado                       | ¿Cuándo se usa? |
|---------------|-------------------------------|----------------------------------|--------------------|
| **In-order**    | izquierda, nodo, derecha     | `20 30 40 50 60 70 80` (¡ordenado en un BST!) | Obtener los elementos ordenados; validar el invariante de BST. |
| **Pre-order**   | nodo, izquierda, derecha     | `50 30 20 40 70 60 80`          | Copiar/serializar un árbol (permite reconstruirlo). |
| **Post-order**  | izquierda, derecha, nodo     | `20 40 30 60 80 70 50`          | Liberar memoria de forma segura (hijos antes que el padre); evaluar árboles de expresión. |
| **Level-order** | por niveles (BFS), izq. a der.| `50 30 70 20 40 60 80`          | Explorar el árbol "capa por capa"; encontrar el nodo más cercano a la raíz. |

Los tres primeros (in/pre/post-order) son recursivos por naturaleza
(DFS - depth-first search) y se implementan de forma natural con recursión o
una pila explícita. Level-order es BFS (breadth-first search) y requiere una
cola (`std::queue`), no una pila.

## 6. Ejercicios de extensión

1. **Validar si un árbol es un BST válido.** Escribe una función
   `bool is_valid_bst(const BinaryTree<int>& tree)` que reciba un
   `dsa::BinaryTree<int>` (que no impone ningún orden) y determine si su
   forma actual cumple el invariante de BST. Pista: no basta con comparar
   cada nodo solo con sus hijos directos; hay que llevar un rango
   `(min, max)` válido que se va estrechando al bajar por el árbol.

2. **Balancear un BST degenerado.** Dado un `BinarySearchTree<T>` que
   degeneró en una lista enlazada (por ejemplo, tras insertar datos ya
   ordenados), escribe una función que lo reconstruya balanceado en O(n):
   (a) extrae todos los valores con un recorrido in-order (ya vienen
   ordenados), y (b) reinserta recursivamente el elemento central del rango
   como raíz de cada subárbol, dividiendo el rango en dos mitades. ¿Qué
   altura tiene el árbol resultante en función de `n`?

3. **Implementar un AVL (árbol auto-balanceado).** No lo implementes en este
   momento, pero plantea el diseño: un AVL es un BST que mantiene además,
   en cada nodo, el **factor de balance** (altura del subárbol
   derecho - altura del subárbol izquierdo, o viceversa) y garantiza que
   ese factor esté siempre en `{-1, 0, 1}`. Cuando una inserción o
   eliminación rompe esa condición, se aplican **rotaciones** (simples o
   dobles: izquierda, derecha, izquierda-derecha, derecha-izquierda) para
   restaurar el balance. ¿Qué método de `BinarySearchTree<T>` cambiaría más
   (`insert` o `erase`)? ¿Por qué una rotación es O(1) en vez de O(n)?

4. *(Opcional, más avanzado)* Añade un iterador in-order **bidireccional**
   (que también soporte `operator--`) a `BinarySearchTree<T>::const_iterator`,
   simétrico al `successor` ya implementado, usando un `predecessor` que
   recorra hacia el hijo izquierdo o hacia arriba por la derecha.
