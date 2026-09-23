# Hitos del curso

Cada hito corresponde a un módulo de la librería. Al final del curso, la unión
de todos los hitos es una librería de estructuras de datos completa, probada
y documentada.

| # | Módulo | Carpeta (`include/dsa/...`) | Guía |
|---|--------|------------------------------|------|
| 1 | Arreglo dinámico (`Vector<T>`) | `linear/dynamic_array.hpp` | [docs/guides/01_dynamic_array.md](guides/01_dynamic_array.md) |
| 2 | Listas enlazadas (simple, doble, circular) | `linear/*_linked_list.hpp` | [docs/guides/02_linked_lists.md](guides/02_linked_lists.md) |
| 3 | Pilas, colas y deques | `linear/stack.hpp`, `queue.hpp`, `deque.hpp` | [docs/guides/03_stacks_queues.md](guides/03_stacks_queues.md) |
| 4 | Árboles binarios y BST | `trees/binary_tree.hpp`, `binary_search_tree.hpp` | [docs/guides/04_trees_bst.md](guides/04_trees_bst.md) |
| 5 | Heaps y colas de prioridad | `trees/binary_heap.hpp`, `priority_queue.hpp` | [docs/guides/05_heaps.md](guides/05_heaps.md) |
| 6 | Tablas hash | `hashing/hash_map.hpp`, `hash_set.hpp` | [docs/guides/06_hash_tables.md](guides/06_hash_tables.md) |
| 7 | Grafos y algoritmos de grafos | `graphs/graph.hpp`, `graph_algorithms.hpp` | [docs/guides/07_graphs.md](guides/07_graphs.md) |
| 8 | Ordenamiento y búsqueda | `algorithms/sorting.hpp`, `searching.hpp` | [docs/guides/08_sorting_searching.md](guides/08_sorting_searching.md) |

## Formato pedagógico

- **Rama `main`**: versión "esqueleto". Las firmas, la documentación Doxygen
  y los tests ya están escritos; los métodos clave están marcados con
  `// TODO(estudiante): ...` y lanzan `std::logic_error` hasta que se
  implementan. Los tests fallan a propósito — son la lista de tareas.
- **Rama `solutions`**: implementación de referencia completa, con todos los
  tests en verde. Uso exclusivo del docente (evaluación / material de apoyo),
  no se comparte con estudiantes salvo al cierre del curso.
- Cada módulo tiene una guía en `docs/guides/` con teoría, complejidad
  (tabla Big-O), y ejercicios de extensión más allá del TODO base.

## Flujo de trabajo sugerido por hito

1. Leer la guía del módulo (`docs/guides/NN_*.md`).
2. Revisar los tests en `tests/<categoria>/*_test.cpp` — definen el contrato.
3. Implementar los `TODO` en `include/dsa/<categoria>/*.hpp`.
4. `cmake --build build && ctest --test-dir build --output-on-failure`.
5. Abrir PR contra `main` siguiendo [CONTRIBUTING.md](../CONTRIBUTING.md).
