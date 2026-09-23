# Guía 01: Arreglo Dinámico (`dsa::DynamicArray<T>`)

## 1. Motivación: el problema del arreglo estático

Un arreglo (array) clásico en C/C++, como `int arr[10];`, tiene un tamaño **fijo**
decidido en tiempo de compilación (o, con `new int[n]`, en tiempo de ejecución
pero inmutable después). Esto es excelente para el acceso: `arr[i]` es O(1)
porque la dirección de memoria del elemento `i` se calcula directamente como
`base + i * sizeof(T)`.

El problema aparece cuando no sabemos de antemano cuántos elementos vamos a
necesitar. Si el arreglo se llena, no hay forma de "estirarlo": la memoria
justo después de él ya podría estar ocupada por otra cosa. La única opción es
pedir un bloque de memoria nuevo, más grande, y copiar todo lo anterior ahí.

Un **arreglo dinámico** (como `std::vector` en C++, `ArrayList` en Java o
`list` en Python) automatiza exactamente ese proceso: mantiene un buffer
contiguo en el heap, y cuando se llena, reserva un buffer más grande, mueve
los elementos, y libera el buffer viejo. La pregunta clave de esta guía es:
**¿cuánto debe crecer el buffer cada vez?**

## 2. La estrategia: crecimiento geométrico (doblar la capacidad)

Nuestra implementación en `include/dsa/linear/dynamic_array.hpp` distingue
dos cantidades:

- `size()`: cuántos elementos hay realmente almacenados.
- `capacity()`: cuántos elementos caben en el buffer actual sin necesidad de
  reasignar memoria.

Cuando `size() == capacity()` y llega un nuevo `push_back`, se dispara
`grow()`, que **duplica** la capacidad (o la pone en 1 si estaba en 0),
reserva un buffer nuevo, mueve los elementos existentes con
`std::move`, destruye los objetos viejos y libera el buffer anterior.

La alternativa ingenua sería crecer de a un elemento por vez (capacidad
`n → n + 1`). Funciona, pero es mucho más lenta. Vamos a demostrar por qué.

## 3. Análisis amortizado: por qué doblar da O(1) amortizado

**Definición informal:** el *costo amortizado* de una operación es el costo
total de una secuencia de `n` operaciones, dividido entre `n`. No nos importa
que una operación individual sea cara de vez en cuando; nos importa que el
promedio sobre una secuencia larga sea bajo.

### 3.1 El argumento con crecimiento por 1 (por qué falla)

Si cada vez que se llena el buffer lo agrandamos en exactamente 1 slot,
entonces al hacer `n` `push_back` consecutivos desde un arreglo vacío,
**cada** inserción dispara una reasignación (copiar/mover todos los elementos
existentes). El costo total es:

```
1 + 2 + 3 + ... + n = n(n+1)/2 = O(n²)
```

Costo amortizado por operación: `O(n² / n) = O(n)`. Es decir, cada
`push_back` cuesta, en promedio, tiempo lineal en el tamaño del arreglo. Esto
es inaceptable para una operación que "se siente" tan simple como agregar un
elemento al final.

### 3.2 El argumento con duplicación (por qué funciona)

Ahora supongamos que doblamos la capacidad cada vez que se llena: `1, 2, 4,
8, 16, ..., 2^k`. Al hacer `n` inserciones, las reasignaciones ocurren solo
cuando el tamaño pasa por una potencia de 2. El costo de mover elementos en
cada reasignación es proporcional al tamaño del buffer en ese momento. El
costo total de mover elementos a través de todas las reasignaciones es:

```
1 + 2 + 4 + 8 + ... + 2^k  donde 2^k ≈ n
```

Esta es una serie geométrica, y su suma está acotada por:

```
2^(k+1) - 1 < 2n
```

Es decir, el costo total de **todas** las reasignaciones a lo largo de `n`
inserciones es `O(n)`, no `O(n²)`. Sumando el costo de las `n` inserciones
"normales" (cada una O(1) cuando no hay reasignación), el costo total de la
secuencia completa es `O(n) + O(n) = O(n)`.

Costo amortizado por operación: `O(n / n) = O(1)`.

**Intuición clave:** cada vez que reasignamos, "pagamos" para mover todos los
elementos actuales, pero a cambio nos ganamos espacio para el **doble** de
inserciones futuras sin volver a pagar. Las reasignaciones se vuelven
exponencialmente más raras a medida que el arreglo crece, y ese
espaciamiento exponencial es justo lo que cancela el crecimiento lineal del
costo de cada una.

> Este mismo argumento se puede formalizar con el **método del banquero**
> (accounting method): imagina que cada `push_back` "paga" una moneda extra
> además de su costo real de O(1), y esa moneda se guarda en el elemento
> insertado. Cuando ocurre una reasignación de tamaño `m`, se necesitan `m`
> monedas para pagar el movimiento de los `m` elementos, y resulta que
> siempre hay suficientes monedas ahorradas para cubrirlo. Si el factor de
> crecimiento fuera 1 en vez de 2 (o cualquier constante `> 1`), el argumento
> se rompe y el análisis vuelve a dar O(n) amortizado.

## 4. Tabla de complejidad (Big-O)

| Operación                     | Complejidad          | Notas |
|--------------------------------|-----------------------|-------|
| `operator[]` / `at()`          | O(1)                  | Acceso directo por índice, memoria contigua. |
| `front()` / `back()`           | O(1)                  | |
| `push_back`                    | O(1) amortizado, O(n) peor caso | El peor caso ocurre justo cuando hay que reasignar. |
| `pop_back`                     | O(1)                  | Nunca reasigna (no hace *shrink* automático). |
| `insert(index, v)`             | O(n)                  | Peor caso al insertar al inicio (hay que desplazar todo). O(1) si `index == size()`. |
| `erase(index)`                 | O(n)                  | Peor caso al borrar el primero. O(1) si es el último (equivalente a `pop_back`). |
| `size()` / `capacity()` / `empty()` | O(1)             | |
| `clear()`                      | O(n)                  | Debe destruir cada elemento (llamar a su destructor). |
| Copia (constructor/asignación) | O(n)                  | Copia elemento por elemento. |
| Movimiento (constructor/asignación) | O(1)             | Solo se intercambian punteros. |
| Iteración completa (`begin`→`end`) | O(n)              | |

## 5. Comparación con listas enlazadas (adelanto)

Más adelante en el curso van a implementar `LinkedList<T>`. Aquí una
comparación rápida para que entiendan **por qué existen ambas** estructuras:

| Aspecto                        | `DynamicArray<T>`        | Lista enlazada simple |
|---------------------------------|---------------------------|------------------------|
| Acceso por índice `arr[i]`      | O(1)                      | O(n) (hay que recorrer desde el inicio) |
| Inserción/borrado al final      | O(1) amortizado           | O(1) si se mantiene un puntero `tail`, O(n) si no |
| Inserción/borrado al inicio     | O(n) (desplazar todo)     | O(1) |
| Inserción/borrado en medio      | O(n)                      | O(n) para encontrar la posición, pero O(1) una vez ahí (solo re-enlazar) |
| Localidad de memoria (caché)    | Excelente (contigua)      | Mala (nodos dispersos en el heap) |
| Overhead de memoria por elemento| Ninguno (solo el dato)    | Un puntero extra por nodo (o dos si es doble) |
| Uso típico                      | Cuando se necesita acceso aleatorio frecuente, o iterar mucho | Cuando hay muchas inserciones/borrados en los extremos o se necesitan referencias estables a elementos |

La lección de fondo: **no existe una estructura de datos universalmente
mejor**. `DynamicArray` gana en acceso aleatorio y en uso de caché; la lista
enlazada gana en inserciones/borrados en posiciones arbitrarias sin
desplazar memoria. Elegir la estructura correcta depende del patrón de uso.

## 6. Detalles de implementación que vale la pena notar

- **Memoria cruda vs. objetos construidos**: el buffer se reserva con
  `::operator new(n * sizeof(T))`, que asigna memoria *sin* construir
  objetos `T`. Los elementos se construyen explícitamente con
  *placement new* (`new (data_ + i) T(...)`) solo en las posiciones
  `[0, size())`. Esto es exactamente lo que hace `std::vector` internamente
  (con `std::allocator`), y es la razón por la cual `capacity() >= size()`
  no significa que haya "basura" de tipo `T` en las posiciones no usadas:
  esas posiciones son memoria cruda, no objetos.
- **Regla de los 5**: como la clase maneja memoria manualmente, debe definir
  explícitamente destructor, constructor/asignación de copia y
  constructor/asignación de movimiento. El movimiento es O(1) porque solo
  intercambia punteros (`data_`, `size_`, `capacity_`), dejando al objeto
  origen vacío.
- **`insert`/`erase` con `std::move`**: al desplazar elementos, se usa
  `std::move` en vez de copiar, para aprovechar movimientos baratos en tipos
  como `std::string` o `std::vector`.

## 7. Ejercicios de extensión

1. **`shrink_to_fit()`**: implementa un método que reasigne el buffer para
   que `capacity() == size()` exactamente (liberando memoria sobrante).
   Pista: ¿qué pasa si `size() == 0`? ¿Debería `capacity()` quedar en 0 o en
   algún mínimo? Discute el trade-off: ¿por qué `std::vector` no hace esto
   automáticamente después de cada `pop_back` o `erase`?

2. **`insert` de múltiples elementos**: agrega una sobrecarga
   `insert(std::size_t index, const T* first, const T* last)` (o usando
   iteradores genéricos) que inserte un rango de elementos de una sola vez,
   desplazando el resto del arreglo **una sola vez** (no llames a tu
   `insert` de un solo elemento en un bucle, porque eso sería O(n·k) en vez
   de O(n + k) para insertar `k` elementos). Compara la complejidad de tu
   solución con la de insertar uno por uno.

3. **Benchmark de crecimiento vs. `reserve`**: usando `dsa::Timer`
   (`include/dsa/utils/timer.hpp`), escribe un programa que compare el
   tiempo de hacer 1,000,000 de `push_back` (a) sin reservar capacidad
   previa, y (b) llamando primero a un `reserve(1000000)` (si lo
   implementas) o construyendo con `DynamicArray<int>(1000000)`. Corre
   varias repeticiones y reporta el promedio. ¿Qué tan grande es la
   diferencia? Relaciona el resultado con el análisis amortizado de la
   Sección 3: ¿por qué la diferencia es notable pero no de varios órdenes
   de magnitud?

4. *(Opcional, más avanzado)* **Factor de crecimiento distinto de 2**:
   modifica `grow()` para usar un factor de 1.5 en vez de 2 (una técnica
   real que usa, por ejemplo, la implementación de `std::vector` en algunos
   compiladores). Investiga y explica, con tus propias palabras, por qué un
   factor de 1.5 permite reutilizar memoria liberada por reasignaciones
   anteriores más fácilmente que un factor de 2, en el contexto de un
   *allocator* que reutiliza bloques libres.

## 8. Para pensar antes de la siguiente clase

Si `push_back` es O(1) amortizado, ¿por qué `insert(0, v)` (insertar al
inicio) sigue siendo O(n) sin importar la estrategia de crecimiento? ¿Qué
tendría que cambiar en la estructura de datos para que insertar al inicio
también fuera O(1)? (Esta pregunta es un adelanto de por qué, más adelante,
vamos a estudiar `Deque` y listas enlazadas dobles.)
