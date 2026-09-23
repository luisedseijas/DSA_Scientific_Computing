#ifndef DSA_ALGORITHMS_SEARCHING_HPP
#define DSA_ALGORITHMS_SEARCHING_HPP

#include <iterator>
#include <stdexcept>

namespace dsa {

namespace detail {

/**
 * @brief Auxiliar recursivo de busqueda binaria que mantiene el iterador de
 * fin original (`sentinel`) separado del `last` del subrango actual, ya que
 * al reducir la busqueda a la mitad izquierda `last` se achica, pero el
 * caso "no encontrado" debe seguir reportandose con el iterador de fin
 * original del llamador.
 */
template <typename RandomIt, typename T>
RandomIt binary_search_recursive_impl(RandomIt first, RandomIt last, RandomIt sentinel,
                                      const T& value) {
    auto count = std::distance(first, last);
    if (count <= 0) return sentinel;

    RandomIt mid = first;
    std::advance(mid, count / 2);

    if (*mid < value) {
        return binary_search_recursive_impl(mid + 1, last, sentinel, value);
    }
    if (value < *mid) {
        return binary_search_recursive_impl(first, mid, sentinel, value);
    }
    return mid;
}

}  // namespace detail

/**
 * @brief Busca en [first, last) el primer elemento igual a `value`.
 *
 * Funciona con cualquier iterador de entrada; no asume ningun orden.
 *
 * @tparam InputIt Tipo de iterador de entrada.
 * @tparam T Tipo del valor, comparado con operator==.
 * @param first Iterador al comienzo del rango.
 * @param last Iterador a una posicion despues del final del rango.
 * @param value Valor a buscar.
 * @return Iterador al primer elemento coincidente, o `last` si no se
 * encuentra.
 *
 * Complejidad: O(n) comparaciones en el peor caso; O(1) en el mejor caso.
 */
template <typename InputIt, typename T>
InputIt linear_search(InputIt first, InputIt last, const T& value) {
    for (; first != last; ++first) {
        if (*first == value) return first;
    }
    return last;
}

/**
 * @brief Busca `value` en el rango ordenado [first, last) usando busqueda
 * binaria iterativa.
 *
 * Precondicion: [first, last) debe estar ordenado ascendentemente segun
 * operator< (el mismo orden que producen por defecto std::sort y las
 * funciones de ordenamiento de dsa). Pasar un rango no ordenado produce
 * resultados indefinidos/incorrectos.
 *
 * @tparam RandomIt Tipo de iterador de acceso aleatorio.
 * @tparam T Tipo del valor, comparado con operator<.
 * @param first Iterador al comienzo del rango ordenado.
 * @param last Iterador a una posicion despues del final del rango ordenado.
 * @param value Valor a buscar.
 * @return Iterador a un elemento coincidente, o `last` si no se encuentra.
 *
 * Complejidad: O(log n) comparaciones.
 */
template <typename RandomIt, typename T>
RandomIt binary_search(RandomIt /*first*/, RandomIt /*last*/, const T& /*value*/) {
    // TODO(estudiante): reduce iterativamente el rango a la mitad comparando
    // el elemento central con value, hasta encontrarlo o agotar el rango.
    throw std::logic_error("dsa::binary_search: no implementado");
}

/**
 * @brief Variante recursiva de la busqueda binaria, ofrecida como
 * contraparte pedagogica de la version iterativa dsa::binary_search: misma
 * complejidad O(log n) y misma precondicion (rango ordenado), pero se apoya
 * en la pila de llamadas (O(log n) de espacio extra) en lugar de un bucle,
 * lo cual es util para comparar ambos estilos.
 *
 * Precondicion: [first, last) debe estar ordenado ascendentemente.
 *
 * @tparam RandomIt Tipo de iterador de acceso aleatorio.
 * @tparam T Tipo del valor, comparado con operator<.
 * @param first Iterador al comienzo del rango ordenado.
 * @param last Iterador a una posicion despues del final del rango ordenado.
 * @param value Valor a buscar.
 * @return Iterador a un elemento coincidente, o `last` si no se encuentra.
 */
template <typename RandomIt, typename T>
RandomIt binary_search_recursive(RandomIt /*first*/, RandomIt /*last*/, const T& /*value*/) {
    // TODO(estudiante): version recursiva de binary_search; delega en un
    // helper (por ejemplo detail::binary_search_recursive_impl) que compare
    // el elemento central y recurse sobre la mitad correspondiente.
    throw std::logic_error("dsa::binary_search_recursive: no implementado");
}

}  // namespace dsa

#endif  // DSA_ALGORITHMS_SEARCHING_HPP
