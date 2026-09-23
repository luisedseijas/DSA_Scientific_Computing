#ifndef DSA_INTERFACES_COLLECTION_HPP
#define DSA_INTERFACES_COLLECTION_HPP

#include <cstddef>

namespace dsa {

/**
 * @brief Contrato base que implementa todo contenedor de esta librería DSA.
 *
 * Se mantiene mínimo a propósito: solo captura lo que todo contenedor
 * de esta librería tiene en común (un tamaño y una verificación de
 * vacuidad). Los contratos ADT concretos (List, Stack, Queue, Tree, ...)
 * se construyen sobre este.
 */
template <typename T>
class Collection {
   public:
    virtual ~Collection() = default;

    virtual std::size_t size() const noexcept = 0;

    virtual bool empty() const noexcept { return size() == 0; }

    virtual void clear() = 0;
};

}  // namespace dsa

#endif  // DSA_INTERFACES_COLLECTION_HPP
