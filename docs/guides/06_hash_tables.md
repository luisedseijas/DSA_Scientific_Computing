# Módulo 6 — Tablas hash

`include/dsa/hashing/hash_map.hpp` — `dsa::HashMap<Key, Value, Hash>`
`include/dsa/hashing/hash_set.hpp` — `dsa::HashSet<Key, Hash>`

## 1. La idea central: acceso "casi O(1)"

Una tabla hash busca ofrecer inserción, búsqueda y eliminación en tiempo
promedio **O(1)**, algo que ninguna de las estructuras vistas hasta ahora
(arreglos, listas, árboles) logra para búsquedas por clave arbitraria. El
truco es no buscar la clave comparándola una por una, sino **calcular
directamente** en qué posición debería estar.

Eso es lo que hace una **función hash**: `h(clave) -> índice de bucket`.
En vez de recorrer la estructura, evaluamos `h(clave) % número_de_buckets`
y vamos directo a esa posición. Si `h` distribuye las claves de forma
razonablemente uniforme sobre el rango de salida, en promedio cada
posición ("bucket") tendrá muy pocos elementos, y comparar contra esos
pocos elementos es O(1) amortizado.

En este módulo usamos `std::hash<Key>` como función hash por defecto
(la biblioteca estándar ya provee implementaciones razonables para `int`,
`std::string`, etc.), pero la clase acepta cualquier funtor `Hash` como
parámetro de plantilla — así los estudiantes pueden experimentar con
funciones hash propias (ver ejercicios).

### Propiedades deseables de una función hash

1. **Determinismo**: la misma clave siempre produce el mismo hash.
2. **Uniformidad**: claves distintas deberían distribuirse parejo sobre el
   rango de salida (evitar que todas caigan en el mismo bucket).
3. **Eficiencia**: calcular el hash debe ser rápido (idealmente O(longitud
   de la clave), no más).
4. **Consistencia con `operator==`**: si `a == b`, entonces
   `h(a) == h(b)` (obligatorio); lo contrario no tiene que cumplirse
   (dos claves distintas *pueden* colisionar).

## 2. Colisiones: el problema inevitable

Por el [principio del palomar](https://es.wikipedia.org/wiki/Principio_del_palomar),
si hay más claves posibles que buckets (que es casi siempre el caso),
**tarde o temprano dos claves distintas van a mapear al mismo índice**.
Esto se llama una **colisión**, y toda tabla hash necesita una estrategia
para resolverlas. Hay dos familias principales:

### 2.1 Separate chaining (lo que implementamos aquí)

Cada bucket no guarda un único elemento, sino una **colección** de
elementos (una lista o, como en este módulo, un `std::vector`). Cuando dos
claves colisionan, simplemente conviven en el mismo bucket, y buscar una
clave implica: (1) calcular su bucket, (2) recorrer linealmente ese
bucket comparando claves con `operator==`.

**Decisión de diseño de este módulo**: cada bucket es un
`std::vector<std::pair<Key, Value>>` (en `HashMap`) o un `std::vector<Key>`
(en `HashSet`), en vez de una lista enlazada manual. En un bucket bien
dimensionado (factor de carga controlado, ver §3) hay pocos elementos, así
que la ventaja clásica de una lista enlazada — inserción/eliminación O(1)
sin desplazar memoria — no compensa el peor *cache locality* de recorrer
punteros dispersos en el heap. Un `vector` pequeño, contiguo en memoria,
suele ser más rápido en la práctica moderna, aunque `erase` en su interior
sea técnicamente O(tamaño del bucket) por el corrimiento de elementos. Es
un punto de discusión clásico en el curso: *Big-O no es lo único que
importa* — la localidad de memoria también.

`hash_set.hpp` **no incluye** `hash_map.hpp`: aunque la estrategia es
idéntica, se reimplementa de forma independiente para que cada archivo se
pueda leer de manera autocontenida, sin tener que saltar entre dos
archivos para entender uno solo.

### 2.2 Open addressing (contraste, NO implementado aquí)

La alternativa es no permitir múltiples elementos por bucket: cada bucket
guarda a lo sumo un elemento, y ante una colisión se **prueba otra
posición** dentro del mismo arreglo, siguiendo alguna secuencia de sondeo
(*probing*):

- **Linear probing**: si `h(k)` está ocupado, probar `h(k)+1`,
  `h(k)+2`, ... (módulo el tamaño de la tabla).
- **Quadratic probing**: probar `h(k)+1²`, `h(k)+2²`, ...
- **Double hashing**: usar una segunda función hash para calcular el
  salto entre intentos.

Open addressing evita la indirección de listas/vectores anidados (todo
vive en un único arreglo plano, excelente localidad de caché), pero tiene
sus propias complicaciones: el borrado es más delicado (hay que marcar
"tumbas" en vez de vaciar la celda, para no romper cadenas de búsqueda), y
sufre de *clustering* (grupos de celdas ocupadas que crecen y degradan el
rendimiento) especialmente con linear probing. Queda como ejercicio de
extensión (§5) implementarlo y comparar.

## 3. Factor de carga y rehashing

El **factor de carga** (*load factor*) es:

```
load_factor = número_de_elementos / número_de_buckets
```

Mide, en promedio, cuántos elementos hay por bucket. Si crece sin control
(por ejemplo, insertamos 10,000 claves en una tabla de 8 buckets), cada
bucket termina con cientos de elementos, y la búsqueda dentro del bucket
deja de ser O(1) para volverse O(n) — perdemos toda la ventaja de usar una
tabla hash.

La solución es el **rehashing automático**: cuando `load_factor` supera un
umbral (`kMaxLoadFactor = 0.75` en este módulo, un valor típico también
usado por `std::unordered_map`), la tabla:

1. Duplica el número de buckets.
2. Recalcula el bucket de **cada elemento existente** (el índice depende
   del número de buckets, así que cambia al cambiar ese número).
3. Reinserta todos los elementos en la nueva disposición.

Esta operación es O(n), pero ocurre con muy poca frecuencia (solo cuando
se duplica el tamaño). El argumento de **análisis amortizado** es idéntico
al de `DynamicArray` (Módulo 1, ver `docs/guides/01_dynamic_array.md`):
duplicar en vez de crecer de a un bucket hace que el costo total de N
inserciones sea O(N), es decir, O(1) amortizado por inserción, aunque
inserciones individuales ocasionalmente cuesten O(n).

## 4. Complejidad (Big-O)

| Operación         | Promedio (buena distribución) | Peor caso (muchas colisiones) |
|--------------------|:-----------------------------:|:------------------------------:|
| `insert`           | O(1) amortizado               | O(n)                           |
| `find` / `contains`| O(1)                           | O(n)                           |
| `erase`            | O(1)                           | O(n)                           |
| `operator[]`       | O(1) amortizado               | O(n)                           |
| rehash             | O(n) (ocurre O(log n) veces)   | O(n)                           |

El **peor caso** ocurre cuando la función hash es mala (o un atacante
elige claves adversariamente) y todas las claves colisionan en el mismo
bucket: la tabla hash degenera en una lista enlazada, y todas las
operaciones caen a O(n). Esto es exactamente lo que los tests de este
módulo fuerzan a propósito con `BadHash` (`x % 4`), para verificar que la
tabla sigue siendo *correcta* (aunque lenta) incluso en ese escenario.

## 5. Ejercicios de extensión

1. **Open addressing con linear probing.** Implementa una variante
   `HashMapOpenAddressing<Key, Value, Hash>` que use un único
   `std::vector<std::optional<std::pair<Key, Value>>>` (o un estado
   EMPTY/OCCUPIED/DELETED por celda) en vez de buckets con chaining.
   Presta especial atención a cómo manejas el borrado (¿por qué no basta
   con vaciar la celda?) y a cuándo/cómo rehashear. Compara su rendimiento
   contra `HashMap` con el benchmark script.

2. **Contar y graficar colisiones.** Instrumenta una versión de
   `HashMap` (o una copia local) que cuente, por cada `insert`, cuántas
   comparaciones hizo dentro del bucket antes de encontrar el lugar libre
   (esto aproxima el número de colisiones). Usa
   `benchmarks/performance_plot.py` como referencia para graficar el
   número de colisiones acumuladas en función del número de elementos
   insertados, para distintas funciones hash (`std::hash<int>` vs. una
   función hash deliberadamente mala como `BadHash`). ¿Qué tan distinto
   es el crecimiento?

3. **Tu propia función hash para un struct.** Define un struct propio,
   por ejemplo:
   ```cpp
   struct Point { int x, y; };
   bool operator==(const Point& a, const Point& b) {
     return a.x == b.x && a.y == b.y;
   }
   ```
   e implementa un funtor `PointHash` (por ejemplo combinando
   `std::hash<int>` sobre `x` y `y` con la técnica de `boost::hash_combine`:
   `seed ^= hash(x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);`). Úsalo como
   parámetro `Hash` de `dsa::HashMap<Point, std::string, PointHash>` y
   escribe tests que confirmen que funciona igual que con `int`/`std::string`.
