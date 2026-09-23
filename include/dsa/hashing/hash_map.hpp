#ifndef DSA_HASHING_HASH_MAP_HPP
#define DSA_HASHING_HASH_MAP_HPP

#include <cstddef>
#include <functional>
#include <optional>
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
        maybe_rehash();
        bucket_type& bucket = buckets_[bucket_index(key)];
        for (auto& kv : bucket) {
            if (kv.first == key) {
                kv.second = value;
                return false;
            }
        }
        bucket.emplace_back(key, value);
        ++size_;
        return true;
    }

    /**
     * @brief Accesses the value mapped to `key`, default-constructing an
     * entry for it if it does not already exist (like `std::unordered_map`).
     */
    Value& operator[](const Key& key) {
        maybe_rehash();
        bucket_type& bucket = buckets_[bucket_index(key)];
        for (auto& kv : bucket) {
            if (kv.first == key) return kv.second;
        }
        bucket.emplace_back(key, Value{});
        ++size_;
        return bucket.back().second;
    }

    /** @brief Returns a pointer to the value for `key`, or nullptr if absent. */
    Value* find(const Key& key) {
        bucket_type& bucket = buckets_[bucket_index(key)];
        for (auto& kv : bucket) {
            if (kv.first == key) return &kv.second;
        }
        return nullptr;
    }

    /** @brief Const overload of find(). */
    const Value* find(const Key& key) const {
        const bucket_type& bucket = buckets_[bucket_index(key)];
        for (const auto& kv : bucket) {
            if (kv.first == key) return &kv.second;
        }
        return nullptr;
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
        bucket_type& bucket = buckets_[bucket_index(key)];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->first == key) {
                bucket.erase(it);
                --size_;
                return true;
            }
        }
        return false;
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
        if (load_factor() <= kMaxLoadFactor) return;
        std::vector<bucket_type> old_buckets = std::move(buckets_);
        buckets_.assign(old_buckets.size() * 2, bucket_type{});
        for (const bucket_type& bucket : old_buckets) {
            for (const auto& kv : bucket) {
                buckets_[bucket_index(kv.first)].push_back(kv);
            }
        }
    }

    std::vector<bucket_type> buckets_;
    std::size_t size_;
};

}  // namespace dsa

#endif  // DSA_HASHING_HASH_MAP_HPP
