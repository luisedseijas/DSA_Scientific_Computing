#ifndef DSA_HASHING_HASH_MAP_HPP
#define DSA_HASHING_HASH_MAP_HPP

#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace dsa {

/**
 * @brief A hash table mapping keys to values, implemented with separate
 * chaining.
 *
 * Design decision: each bucket is a `std::vector<std::pair<Key, Value>>`
 * instead of a hand-rolled singly linked list. Buckets in a well-tuned hash
 * table (load factor kept below ~0.75) hold only a handful of elements, so
 * a vector's contiguous storage and better cache locality beat the pointer
 * chasing of a linked list in practice, while keeping the implementation
 * short and easy to reason about. The trade-off students should notice:
 * erasing from the middle of a bucket vector is O(bucket size) due to the
 * shift, exactly like it would be with a list-based unlink being O(1) but
 * with worse cache behavior overall -- a classic space/time/locality
 * discussion point for the guide.
 *
 * Collisions (two keys hashing to the same bucket index) are resolved by
 * simply appending to that bucket's vector and scanning it linearly on
 * lookup/erase. When the load factor (size / bucket_count) exceeds
 * `kMaxLoadFactor`, the table rehashes: it doubles the bucket count and
 * reinserts every element into the new bucket array. This is what keeps
 * insert/find/erase O(1) *amortized* even as the table grows -- see
 * docs/guides/06_hash_tables.md for the full argument.
 *
 * @tparam Key Key type. Must be usable with `Hash` and `operator==`.
 * @tparam Value Mapped type. Must be default constructible (used by
 * `operator[]`).
 * @tparam Hash Hash functor, defaults to `std::hash<Key>`.
 */
template <typename Key, typename Value, typename Hash = std::hash<Key>>
class HashMap {
   public:
    using value_type = std::pair<Key, Value>;
    using bucket_type = std::vector<value_type>;

    /** @brief Load factor threshold that triggers an automatic rehash. */
    static constexpr double kMaxLoadFactor = 0.75;

    /** @brief Constructs an empty map with `bucket_count` initial buckets. */
    explicit HashMap(std::size_t bucket_count = 8)
        : buckets_(bucket_count == 0 ? 1 : bucket_count), size_(0) {}

    /**
     * @brief Inserts `key` with `value`, or overwrites the value of an
     * existing entry with the same key.
     * @return true if a new entry was inserted, false if an existing one was
     * overwritten.
     */
    bool insert(const Key& key, const Value& value) {
        (void)key;
        (void)value;
        // TODO(estudiante): si `key` ya existe, sobrescribe su valor y
        // devuelve false; si no, agrégala al bucket correspondiente,
        // incrementa size_ y devuelve true. No olvides llamar a
        // maybe_rehash() antes de insertar.
        throw std::logic_error("HashMap::insert: no implementado");
    }

    /**
     * @brief Accesses the value mapped to `key`, default-constructing an
     * entry for it if it does not already exist (like `std::unordered_map`).
     */
    Value& operator[](const Key& key) {
        (void)key;
        // TODO(estudiante): si `key` ya existe, devuelve una referencia a su
        // valor; si no, crea una entrada con Value{} en el bucket
        // correspondiente, incrementa size_ y devuelve una referencia a
        // ella. No olvides llamar a maybe_rehash() antes de insertar.
        throw std::logic_error("HashMap::operator[]: no implementado");
    }

    /** @brief Returns a pointer to the value for `key`, or nullptr if absent. */
    Value* find(const Key& key) {
        (void)key;
        // TODO(estudiante): recorre el bucket correspondiente a `key` y
        // devuelve un puntero a su valor si lo encuentras, o nullptr si no
        // está presente.
        throw std::logic_error("HashMap::find: no implementado");
    }

    /** @brief Const overload of find(). */
    const Value* find(const Key& key) const {
        (void)key;
        // TODO(estudiante): igual que la sobrecarga no-const, pero sobre un
        // bucket const y devolviendo un puntero const.
        throw std::logic_error("HashMap::find: no implementado");
    }

    /** @brief Returns a copy of the value for `key`, if present. */
    std::optional<Value> get(const Key& key) const {
        const Value* found = find(key);
        if (found == nullptr) return std::nullopt;
        return *found;
    }

    /** @brief Returns true if `key` is present in the map. */
    bool contains(const Key& key) const { return find(key) != nullptr; }

    /**
     * @brief Removes the entry for `key`, if present.
     * @return true if an entry was removed, false if `key` was not found.
     */
    bool erase(const Key& key) {
        (void)key;
        // TODO(estudiante): busca `key` en su bucket; si la encuentras,
        // elimínala del vector, decrementa size_ y devuelve true. Si no
        // está presente, devuelve false.
        throw std::logic_error("HashMap::erase: no implementado");
    }

    /** @brief Number of key-value entries stored in the map. */
    std::size_t size() const noexcept { return size_; }

    /** @brief True if the map has no entries. */
    bool empty() const noexcept { return size_ == 0; }

    /** @brief Removes all entries and shrinks back to a single bucket. */
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
        // reinserta cada entrada existente en el nuevo arreglo de buckets
        // (recalculando su índice con el nuevo bucket_count).
        throw std::logic_error("HashMap::maybe_rehash: no implementado");
    }

    std::vector<bucket_type> buckets_;
    std::size_t size_;
};

}  // namespace dsa

#endif  // DSA_HASHING_HASH_MAP_HPP
