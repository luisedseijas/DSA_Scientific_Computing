#ifndef DSA_ALGORITHMS_SORTING_HPP
#define DSA_ALGORITHMS_SORTING_HPP

#include <algorithm>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace dsa {

/**
 * @brief Ordena el rango [first, last) usando bubble sort.
 *
 * Recorre repetidamente el rango, intercambiando elementos adyacentes que
 * esten fuera de orden. Termina anticipadamente en cuanto una pasada
 * completa no realiza ningun intercambio.
 *
 * @tparam RandomIt Tipo de iterador de acceso aleatorio.
 * @tparam Compare Tipo del predicado de orden estricto debil.
 * @param first Iterador al comienzo del rango.
 * @param last Iterador a una posicion despues del final del rango.
 * @param comp Comparador; por defecto std::less (orden ascendente).
 *
 * Complejidad: O(n^2) comparaciones/intercambios en el peor caso y en el
 * caso promedio, O(n) en el mejor caso (ya ordenado, gracias a la salida
 * anticipada).
 * Estable: si (solo intercambia elementos adyacentes que esten estrictamente
 * fuera de orden).
 */
template <typename RandomIt,
          typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void bubble_sort(RandomIt /*first*/, RandomIt /*last*/, Compare /*comp*/ = Compare()) {
    // TODO(estudiante): recorre repetidamente el rango intercambiando pares
    // adyacentes fuera de orden, hasta que una pasada completa no haga swaps.
    throw std::logic_error("dsa::bubble_sort: no implementado");
}

/**
 * @brief Ordena el rango [first, last) usando insertion sort.
 *
 * Construye el rango ordenado de forma incremental, insertando cada
 * elemento en su posicion correcta dentro del prefijo ya ordenado.
 *
 * @tparam RandomIt Tipo de iterador de acceso aleatorio.
 * @tparam Compare Tipo del predicado de orden estricto debil.
 * @param first Iterador al comienzo del rango.
 * @param last Iterador a una posicion despues del final del rango.
 * @param comp Comparador; por defecto std::less (orden ascendente).
 *
 * Complejidad: O(n^2) en el peor caso/caso promedio, O(n) en el mejor caso
 * (ya ordenado).
 * Estable: si.
 */
template <typename RandomIt,
          typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void insertion_sort(RandomIt /*first*/, RandomIt /*last*/, Compare /*comp*/ = Compare()) {
    // TODO(estudiante): construye el rango ordenado incrementalmente,
    // insertando cada elemento en su posicion correcta dentro del prefijo ya
    // ordenado.
    throw std::logic_error("dsa::insertion_sort: no implementado");
}

/**
 * @brief Ordena el rango [first, last) usando selection sort.
 *
 * Selecciona repetidamente el elemento minimo (segun comp) del sufijo no
 * ordenado y lo intercambia hacia su posicion correcta.
 *
 * @tparam RandomIt Tipo de iterador de acceso aleatorio.
 * @tparam Compare Tipo del predicado de orden estricto debil.
 * @param first Iterador al comienzo del rango.
 * @param last Iterador a una posicion despues del final del rango.
 * @param comp Comparador; por defecto std::less (orden ascendente).
 *
 * Complejidad: O(n^2) en todos los casos (mejor, promedio y peor), ya que
 * el sufijo no ordenado siempre se recorre por completo sin importar su
 * orden.
 * Estable: no (el intercambio puede mover un elemento igual mas alla de
 * otro elemento igual).
 */
template <typename RandomIt,
          typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void selection_sort(RandomIt /*first*/, RandomIt /*last*/, Compare /*comp*/ = Compare()) {
    // TODO(estudiante): en cada iteracion, busca el minimo del sufijo no
    // ordenado (segun comp) e intercambialo con el inicio de ese sufijo.
    throw std::logic_error("dsa::selection_sort: no implementado");
}

namespace detail {

/**
 * @brief Fusiona dos subrangos ordenados adyacentes [first, mid) y
 * [mid, last) en un unico rango ordenado usando un buffer auxiliar.
 */
template <typename RandomIt, typename Compare>
void merge_ranges(RandomIt first, RandomIt mid, RandomIt last, Compare comp) {
    using ValueType = typename std::iterator_traits<RandomIt>::value_type;
    std::vector<ValueType> buffer;
    buffer.reserve(static_cast<std::size_t>(std::distance(first, last)));

    RandomIt left = first;
    RandomIt right = mid;
    while (left != mid && right != last) {
        if (comp(*right, *left)) {
            buffer.push_back(std::move(*right));
            ++right;
        } else {
            buffer.push_back(std::move(*left));
            ++left;
        }
    }
    while (left != mid) {
        buffer.push_back(std::move(*left));
        ++left;
    }
    while (right != last) {
        buffer.push_back(std::move(*right));
        ++right;
    }
    std::move(buffer.begin(), buffer.end(), first);
}

}  // namespace detail

/**
 * @brief Ordena el rango [first, last) usando merge sort recursivo.
 *
 * Divide el rango a la mitad, ordena recursivamente cada mitad y luego
 * fusiona las dos mitades ordenadas usando un buffer auxiliar (ver
 * detail::merge_ranges).
 *
 * @tparam RandomIt Tipo de iterador de acceso aleatorio.
 * @tparam Compare Tipo del predicado de orden estricto debil.
 * @param first Iterador al comienzo del rango.
 * @param last Iterador a una posicion despues del final del rango.
 * @param comp Comparador; por defecto std::less (orden ascendente).
 *
 * Complejidad: O(n log n) en el mejor, promedio y peor caso.
 * Espacio: O(n) auxiliar debido al buffer de fusion.
 * Estable: si (el paso de fusion prefiere el subrango izquierdo en caso de
 * empate).
 */
template <typename RandomIt,
          typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void merge_sort(RandomIt /*first*/, RandomIt /*last*/, Compare /*comp*/ = Compare()) {
    // TODO(estudiante): divide el rango a la mitad, ordena recursivamente
    // cada mitad y fusionalas con detail::merge_ranges.
    throw std::logic_error("dsa::merge_sort: no implementado");
}

namespace detail {

/**
 * @brief Elige un pivote usando la estrategia de mediana de tres (elementos
 * primero, medio y ultimo) y lo mueve a (last - 1), una disposicion
 * compatible con Hoare/Lomuto. Esto evita el comportamiento O(n^2) en el
 * peor caso ante entradas ya ordenadas o en orden inverso, algo que si sufre
 * una estrategia ingenua de "siempre elegir el ultimo elemento".
 */
template <typename RandomIt, typename Compare>
RandomIt median_of_three_pivot(RandomIt first, RandomIt last, Compare comp) {
    RandomIt mid = first + std::distance(first, last) / 2;
    RandomIt end = last - 1;
    if (comp(*mid, *first)) std::iter_swap(mid, first);
    if (comp(*end, *first)) std::iter_swap(end, first);
    if (comp(*end, *mid)) std::iter_swap(end, mid);
    // *mid ahora contiene la mediana de {first, mid, end}. Se mueve junto al
    // final para poder usarla como pivote de la particion de Lomuto.
    std::iter_swap(mid, end);
    return end;
}

template <typename RandomIt, typename Compare>
void quick_sort_impl(RandomIt first, RandomIt last, Compare comp) {
    auto distance = std::distance(first, last);
    if (distance <= 1) return;
    if (distance <= 16) {
        // Rangos pequenos: insertion sort evita el costo de la recursion y en
        // la practica es rapido para particiones diminutas casi ordenadas.
        insertion_sort(first, last, comp);
        return;
    }

    RandomIt pivot_it = median_of_three_pivot(first, last, comp);
    auto pivot_value = *pivot_it;
    RandomIt store = first;
    for (RandomIt it = first; it != pivot_it; ++it) {
        if (comp(*it, pivot_value)) {
            std::iter_swap(it, store);
            ++store;
        }
    }
    std::iter_swap(store, pivot_it);

    quick_sort_impl(first, store, comp);
    quick_sort_impl(store + 1, last, comp);
}

}  // namespace detail

/**
 * @brief Ordena el rango [first, last) usando quick sort.
 *
 * Usa seleccion de pivote por mediana de tres para evitar la degradacion
 * clasica a O(n^2) que sufre un quicksort de pivote fijo ante entradas ya
 * ordenadas o en orden inverso, y recurre a insertion sort para particiones
 * pequenas.
 *
 * @tparam RandomIt Tipo de iterador de acceso aleatorio.
 * @tparam Compare Tipo del predicado de orden estricto debil.
 * @param first Iterador al comienzo del rango.
 * @param last Iterador a una posicion despues del final del rango.
 * @param comp Comparador; por defecto std::less (orden ascendente).
 *
 * Complejidad: O(n log n) en promedio y (con mediana de tres) tipicamente
 * tambien en el peor caso en la practica; el verdadero peor caso sigue
 * siendo O(n^2) para entradas adversariales, aunque la mediana de tres hace
 * seguras las entradas naturalmente ordenadas.
 * Estable: no (la particion intercambia elementos a traves del pivote).
 */
template <typename RandomIt,
          typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void quick_sort(RandomIt /*first*/, RandomIt /*last*/, Compare /*comp*/ = Compare()) {
    // TODO(estudiante): elige un pivote, particiona el rango alrededor de el
    // y ordena recursivamente las dos particiones resultantes.
    throw std::logic_error("dsa::quick_sort: no implementado");
}

namespace detail {

/**
 * @brief Restaura la propiedad de max-heap (segun comp) para el subarbol con
 * raiz en el indice `root` dentro del arreglo logico
 * [first, first + heap_size), asumiendo que ambos subarboles hijos ya
 * satisfacen la propiedad de heap.
 */
template <typename RandomIt, typename Compare>
void sift_down(RandomIt first, std::size_t heap_size, std::size_t root, Compare comp) {
    while (true) {
        std::size_t left = 2 * root + 1;
        std::size_t right = 2 * root + 2;
        std::size_t largest = root;

        if (left < heap_size && comp(*(first + largest), *(first + left))) {
            largest = left;
        }
        if (right < heap_size && comp(*(first + largest), *(first + right))) {
            largest = right;
        }
        if (largest == root) return;

        std::iter_swap(first + root, first + largest);
        root = largest;
    }
}

}  // namespace detail

/**
 * @brief Ordena el rango [first, last) usando heap sort.
 *
 * Construye un max-heap binario in situ (segun comp) sobre el rango y luego
 * extrae repetidamente el maximo, colocandolo al final. Es una
 * implementacion autocontenida de heapify sobre iteradores; no depende de
 * dsa::BinaryHeap.
 *
 * @tparam RandomIt Tipo de iterador de acceso aleatorio.
 * @tparam Compare Tipo del predicado de orden estricto debil.
 * @param first Iterador al comienzo del rango.
 * @param last Iterador a una posicion despues del final del rango.
 * @param comp Comparador; por defecto std::less (orden ascendente).
 *
 * Complejidad: O(n log n) en el mejor, promedio y peor caso.
 * Espacio: O(1) auxiliar (ordena in situ).
 * Estable: no (la extraccion del heap reordena elementos iguales).
 */
template <typename RandomIt,
          typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void heap_sort(RandomIt /*first*/, RandomIt /*last*/, Compare /*comp*/ = Compare()) {
    // TODO(estudiante): construye un max-heap en el rango con detail::sift_down
    // y luego extrae repetidamente el maximo hacia el final del rango.
    throw std::logic_error("dsa::heap_sort: no implementado");
}

}  // namespace dsa

#endif  // DSA_ALGORITHMS_SORTING_HPP
