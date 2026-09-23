#ifndef DSA_HASHING_HASH_SET_HPP
#define DSA_HASHING_HASH_SET_HPP

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <vector>

namespace dsa {

/**
 * @brief A hash-based set of unique keys, implemented with separate
 * chaining.
 *
 * This is intentionally self-contained and does not reuse HashMap: it is
 * implemented independently (each bucket is a `std::vector<Key>` rather
 * than a `std::vector<std::pair<Key, Value>>`) so that it can be read on
 * its own, pedagogically, without having to first understand hash_map.hpp.
 * Both files share the same strategy (see hash_map.hpp for the fuller
 * discussion of why chaining uses a vector-per-bucket and why rehashing
 * keeps operations O(1) amortized).
 *
 * @tparam Key Key type. Must be usable with `Hash` and `operator==`.
 * @tparam Hash Hash functor, defaults to `std::hash<Key>`.
 */
template <typename Key, typename Hash = std::hash<Key>>
class HashSet {
   public:
    using bucket_type = std::vector<Key>;

    /** @brief Load factor threshold that triggers an automatic rehash. */
    static constexpr double kMaxLoadFactor = 0.75;

    /** @brief Constructs an empty set with `bucket_count` initial buckets. */
    explicit HashSet(std::size_t bucket_count = 8)
        : buckets_(bucket_count == 0 ? 1 : bucket_count), size_(0) {}

    /**
     * @brief Inserts `key` if not already present.
     * @return true if `key` was newly inserted, false if it already existed.
     */
    bool insert(const Key& key) {
        (void)key;
        // TODO(estudiante): si `key` ya está presente, devuelve false sin
        // modificar el set; si no, agrégala al bucket correspondiente,
        // incrementa size_ y devuelve true. No olvides llamar a
        // maybe_rehash() antes de insertar.
        throw std::logic_error("HashSet::insert: no implementado");
    }

    /** @brief Returns true if `key` is present in the set. */
    bool contains(const Key& key) const {
        (void)key;
        // TODO(estudiante): recorre el bucket correspondiente a `key` y
        // devuelve true si la encuentras, false si no.
        throw std::logic_error("HashSet::contains: no implementado");
    }

    /**
     * @brief Returns a pointer to the stored key equal to `key`, or nullptr
     * if absent. Useful when Key carries data beyond what participates in
     * equality/hash (e.g. a struct hashed/compared only by one field).
     */
    const Key* find(const Key& key) const {
        (void)key;
        // TODO(estudiante): recorre el bucket correspondiente a `key` y
        // devuelve un puntero al elemento almacenado si lo encuentras, o
        // nullptr si no está presente.
        throw std::logic_error("HashSet::find: no implementado");
    }

    /**
     * @brief Removes `key` from the set, if present.
     * @return true if an element was removed, false if `key` was not found.
     */
    bool erase(const Key& key) {
        (void)key;
        // TODO(estudiante): busca `key` en su bucket; si la encuentras,
        // elimínala del vector, decrementa size_ y devuelve true. Si no
        // está presente, devuelve false.
        throw std::logic_error("HashSet::erase: no implementado");
    }

    /** @brief Number of keys stored in the set. */
    std::size_t size() const noexcept { return size_; }

    /** @brief True if the set has no elements. */
    bool empty() const noexcept { return size_ == 0; }

    /** @brief Removes all elements and shrinks back to a single bucket. */
    void clear() {
        buckets_.clear();
        buckets_.resize(1);
        size_ = 0;
    }

    /** @brief Number of buckets currently allocated. */
    std::size_t bucket_count() const noexcept { return buckets_.size(); }

    /** @brief Current load factor: size() / bucket_count(). */
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
