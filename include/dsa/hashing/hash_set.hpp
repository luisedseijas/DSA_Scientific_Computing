#ifndef DSA_HASHING_HASH_SET_HPP
#define DSA_HASHING_HASH_SET_HPP

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <vector>

namespace dsa {

/**
 * @brief Un conjunto de claves únicas basado en hashing, implementado con
 * encadenamiento separado.
 *
 * Esto es intencionalmente autocontenido y no reutiliza HashMap: está
 * implementado de forma independiente (cada bucket es un `std::vector<Key>`
 * en lugar de un `std::vector<std::pair<Key, Value>>`) para que pueda
 * leerse por sí solo, pedagógicamente, sin tener que entender antes
 * hash_map.hpp. Ambos archivos comparten la misma estrategia (ver
 * hash_map.hpp para la discusión más completa de por qué el encadenamiento
 * usa un vector por bucket y por qué el rehash mantiene las operaciones en
 * O(1) amortizado).
 *
 * @tparam Key Tipo de la clave. Debe ser utilizable con `Hash` y
 * `operator==`.
 * @tparam Hash Functor de hash, por defecto `std::hash<Key>`.
 */
template <typename Key, typename Hash = std::hash<Key>>
class HashSet {
   public:
    using bucket_type = std::vector<Key>;

    /** @brief Umbral de factor de carga que dispara un rehash automático. */
    static constexpr double kMaxLoadFactor = 0.75;

    /** @brief Construye un conjunto vacío con `bucket_count` buckets iniciales. */
    explicit HashSet(std::size_t bucket_count = 8)
        : buckets_(bucket_count == 0 ? 1 : bucket_count), size_(0) {}

    /**
     * @brief Inserta `key` si aún no está presente.
     * @return true si `key` se insertó de nuevo, false si ya existía.
     */
    bool insert(const Key& key) {
        (void)key;
        // TODO(estudiante): si `key` ya está presente, devuelve false sin
        // modificar el set; si no, agrégala al bucket correspondiente,
        // incrementa size_ y devuelve true. No olvides llamar a
        // maybe_rehash() antes de insertar.
        throw std::logic_error("HashSet::insert: no implementado");
    }

    /** @brief Devuelve true si `key` está presente en el conjunto. */
    bool contains(const Key& key) const {
        (void)key;
        // TODO(estudiante): recorre el bucket correspondiente a `key` y
        // devuelve true si la encuentras, false si no.
        throw std::logic_error("HashSet::contains: no implementado");
    }

    /**
     * @brief Devuelve un puntero a la clave almacenada igual a `key`, o
     * nullptr si está ausente. Útil cuando Key contiene datos más allá de
     * los que participan en la igualdad/hash (p. ej. un struct
     * hasheado/comparado solo por un campo).
     */
    const Key* find(const Key& key) const {
        (void)key;
        // TODO(estudiante): recorre el bucket correspondiente a `key` y
        // devuelve un puntero al elemento almacenado si lo encuentras, o
        // nullptr si no está presente.
        throw std::logic_error("HashSet::find: no implementado");
    }

    /**
     * @brief Elimina `key` del conjunto, si está presente.
     * @return true si se eliminó un elemento, false si `key` no se encontró.
     */
    bool erase(const Key& key) {
        (void)key;
        // TODO(estudiante): busca `key` en su bucket; si la encuentras,
        // elimínala del vector, decrementa size_ y devuelve true. Si no
        // está presente, devuelve false.
        throw std::logic_error("HashSet::erase: no implementado");
    }

    /** @brief Número de claves almacenadas en el conjunto. */
    std::size_t size() const noexcept { return size_; }

    /** @brief True si el conjunto no tiene elementos. */
    bool empty() const noexcept { return size_ == 0; }

    /** @brief Elimina todos los elementos y reduce a un solo bucket. */
    void clear() {
        buckets_.clear();
        buckets_.resize(1);
        size_ = 0;
    }

    /** @brief Número de buckets actualmente asignados. */
    std::size_t bucket_count() const noexcept { return buckets_.size(); }

    /** @brief Factor de carga actual: size() / bucket_count(). */
    double load_factor() const noexcept {
        return static_cast<double>(size_) / static_cast<double>(buckets_.size());
    }

   private:
    std::size_t bucket_index(const Key& key) const { return Hash{}(key) % buckets_.size(); }

    void maybe_rehash() {
        // TODO(estudiante): si load_factor() no supera kMaxLoadFactor, no
        // hagas nada. Si lo supera, duplica el número de buckets y
        // reinserta cada clave existente en el nuevo arreglo de buckets
        // (recalculando su índice con el nuevo bucket_count).
        throw std::logic_error("HashSet::maybe_rehash: no implementado");
    }

    std::vector<bucket_type> buckets_;
    std::size_t size_;
};

}  // namespace dsa

#endif  // DSA_HASHING_HASH_SET_HPP
