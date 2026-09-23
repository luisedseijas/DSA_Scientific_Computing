# 08. Algoritmos de ordenamiento y búsqueda

Este módulo (`include/dsa/algorithms/sorting.hpp` y `include/dsa/algorithms/searching.hpp`)
implementa algoritmos clásicos de ordenamiento y búsqueda como **funciones libres genéricas**
sobre iteradores, al estilo de `<algorithm>` de la STL — no como clases. Todas las funciones
de ordenamiento operan sobre un rango semiabierto `[first, last)` de iteradores de acceso
aleatorio y aceptan opcionalmente un comparador `Compare` (por defecto `std::less<T>`, es
decir, orden ascendente).

## Tabla comparativa

| Algoritmo         | Mejor caso   | Caso promedio | Peor caso    | Espacio extra | Estable |
|--------------------|:------------:|:--------------:|:------------:|:--------------:|:-------:|
| `bubble_sort`      | O(n)         | O(n²)          | O(n²)        | O(1)           | Sí      |
| `insertion_sort`   | O(n)         | O(n²)          | O(n²)        | O(1)           | Sí      |
| `selection_sort`   | O(n²)        | O(n²)          | O(n²)        | O(1)           | No      |
| `merge_sort`       | O(n log n)   | O(n log n)     | O(n log n)   | O(n)           | Sí      |
| `quick_sort`       | O(n log n)   | O(n log n)     | O(n²)*       | O(log n)**     | No      |
| `heap_sort`        | O(n log n)   | O(n log n)     | O(n log n)   | O(1)           | No      |

\* Con la estrategia de pivote mediana-de-tres implementada aquí, el peor caso O(n²) requiere
una entrada adversarial construida a propósito; arreglos ya ordenados o en orden inverso
(que sí degradan a una implementación ingenua) se comportan en O(n log n).

\*\* El espacio extra de `quick_sort` es la profundidad de la pila de recursión, no un buffer
de datos como en `merge_sort`.

## ¿Por qué `quick_sort` puede degradar a O(n²) y cómo mitigarlo?

`quick_sort` funciona particionando el rango alrededor de un **pivote**: todo lo menor
queda a la izquierda, todo lo mayor (o igual) a la derecha, y luego se recurre sobre ambas
mitades. Si el pivote elegido siempre resulta ser el mínimo o el máximo del rango (por
ejemplo, si siempre se elige el primer o el último elemento como pivote, y el arreglo de
entrada ya está ordenado o en orden inverso), cada partición solo reduce el tamaño del
problema en 1 elemento en vez de dividirlo aproximadamente a la mitad. Esto produce una
recursión de profundidad O(n) con O(n) trabajo por nivel, es decir, O(n²) en total —
exactamente el peor caso que uno esperaría evitar con un algoritmo "divide y vencerás".

La implementación en `dsa::quick_sort` mitiga esto usando **mediana de tres**
(`detail::median_of_three_pivot`): se comparan el primer elemento, el elemento del medio y
el último elemento del rango, y se elige la mediana de esos tres como pivote. Esto hace que
un arreglo ya ordenado o en orden inverso —el caso patológico más común en la práctica— ya
no produzca particiones desbalanceadas de tamaño 1. Sigue siendo posible construir una
entrada adversarial que degrade el algoritmo a O(n²) (conociendo de antemano la estrategia
de selección de pivote), pero eso requiere un ataque deliberado, no ocurre con datos
"normales". Además, para rangos pequeños (≤ 16 elementos) se usa `insertion_sort` en lugar
de seguir recursando, lo cual reduce la sobrecarga de la recursión sin afectar la
complejidad asintótica.

## ¿Por qué `merge_sort` es estable y `quick_sort` no?

Un algoritmo de ordenamiento es **estable** si dos elementos considerados "iguales" por el
comparador conservan su orden relativo original después de ordenar.

- `merge_sort` es estable porque su paso de fusión (`detail::merge_ranges`) siempre prefiere
  tomar el elemento de la **subrange izquierda** cuando hay un empate
  (`if (comp(*right, *left)) ... else ...`, es decir, solo se toma el de la derecha si es
  estrictamente menor). Como la subrange izquierda contiene elementos que aparecían antes en
  el arreglo original, los empates se resuelven manteniendo el orden original.
- `quick_sort` no es estable porque el paso de partición intercambia elementos
  (`std::iter_swap`) potencialmente muy separados entre sí para colocarlos a cada lado del
  pivote, sin ninguna garantía sobre el orden relativo de elementos iguales. Un elemento
  igual al pivote puede terminar antes o después de otro elemento igual, dependiendo de
  dónde caiga durante el intercambio.

Por la misma razón, `bubble_sort` e `insertion_sort` son estables (solo intercambian/mueven
elementos estrictamente fuera de orden, nunca elementos iguales entre sí), mientras que
`selection_sort` y `heap_sort` no lo son (ambos hacen intercambios (`swap`) que pueden saltar
elementos iguales entre posiciones lejanas).

## Búsqueda binaria y su precondición

`dsa::binary_search(first, last, value)` busca `value` en un rango de iteradores de acceso
aleatorio en **O(log n)**, dividiendo repetidamente el rango a la mitad y descartando la
mitad donde `value` no puede estar.

**Precondición fundamental: el rango `[first, last)` debe estar ordenado ascendentemente**
según el mismo criterio (`operator<`) que se usaría para comparar los elementos. Si el rango
no está ordenado, el algoritmo puede devolver `last` (no encontrado) aunque el valor esté
presente, o viceversa, porque la lógica de "descartar la mitad" asume que si
`*mid < value`, entonces todo el prefijo `[first, mid]` también es menor que `value` — algo
que solo es válido si el rango está ordenado.

`dsa::binary_search_recursive` es una variante recursiva pensada como ejercicio de
comparación con la versión iterativa: tiene la misma complejidad O(log n) y la misma
precondición, pero usa la pila de llamadas (O(log n) de espacio extra) en lugar de un bucle
`while`. Internamente delega en un *helper* (`detail::binary_search_recursive_impl`) que
recibe el iterador de fin **original** por separado del `last` de la subrange actual — esto
es necesario porque, al recursar sobre la mitad izquierda, el `last` de esa llamada recursiva
ya no es el `last` del rango original, y hay que devolver el `last` correcto cuando el
elemento no se encuentra.

## Ejercicios de extensión

1. **Conteo de inversiones con un sort estable.** Implementa una variante de `merge_sort`
   que, además de ordenar, cuente el número de *inversiones* del arreglo original (pares
   `(i, j)` con `i < j` pero `a[i] > a[j]`). Pista: el conteo se puede hacer durante el paso
   de fusión, sumando la cantidad de elementos restantes en la mitad izquierda cada vez que
   se toma un elemento de la mitad derecha. ¿Por qué es imprescindible que la fusión sea
   estable para que el conteo sea correcto?

2. **K-ésimo elemento con quickselect.** Implementa `dsa::kth_element(first, last, k, comp)`
   usando la misma idea de partición que `quick_sort` (puedes reutilizar
   `detail::median_of_three_pivot`), pero recursando solo sobre la mitad que contiene la
   posición `k` en lugar de sobre ambas mitades. Compara experimentalmente su tiempo de
   ejecución contra `dsa::quick_sort(first, last); *(first + k)` para encontrar el k-ésimo
   elemento — ¿cuál es la ventaja asintótica de quickselect (O(n) esperado) frente a ordenar
   todo el arreglo (O(n log n))?

3. **Comparar tiempos reales de los 6 algoritmos.** Usando `dsa::Timer`
   (`include/dsa/utils/timer.hpp`) y el patrón de `benchmarks/sorting_benchmark.cpp`, mide el
   tiempo de `bubble_sort`, `insertion_sort`, `selection_sort`, `merge_sort`, `quick_sort` y
   `heap_sort` sobre: (a) un arreglo aleatorio grande (por ejemplo, 50 000 elementos), (b) un
   arreglo ya ordenado del mismo tamaño, y (c) un arreglo en orden inverso. Grafica los
   resultados (puedes adaptar `benchmarks/performance_plot.py`) y responde: ¿qué algoritmos
   se ven más afectados por el caso "ya ordenado"? ¿Coincide con lo que predice la tabla de
   Big-O de este documento? ¿Por qué `insertion_sort` podría ser competitivo con
   `quick_sort` en arreglos casi ordenados a pesar de tener peor complejidad asintótica en el
   caso promedio?
