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
 * @brief Una tabla hash que asigna claves a valores, implementada con
 * encadenamiento separado.
 *
 * Decisión de diseño: cada bucket es un `std::vector<std::pair<Key, Value>>`
 * en lugar de una lista enlazada simple hecha a mano. En una tabla hash bien
 * ajustada (con el factor de carga mantenido por debajo de ~0.75) los
 * buckets contienen solo un puñado de elementos, así que el almacenamiento
 * contiguo de un vector y su mejor localidad de caché superan en la
 * práctica al recorrido de punteros de una lista enlazada, manteniendo
 * además la implementación corta y fácil de razonar. El compromiso que los
 * estudiantes deberían notar: eliminar del medio de un vector de bucket es
 * O(tamaño del bucket) debido al desplazamiento, tal como lo sería con una
 * lista, donde desenlazar es O(1) pero con peor comportamiento de caché en
 * general -- un punto de discusión clásico de espacio/tiempo/localidad para
 * la guía.
 *
 * Las colisiones (dos claves que producen el mismo índice de bucket) se
 * resuelven simplemente agregando al vector de ese bucket y recorriéndolo
 * linealmente al buscar/eliminar. Cuando el factor de carga (size /
 * bucket_count) supera `kMaxLoadFactor`, la tabla se rehashea: duplica el
 * número de buckets y reinserta cada elemento en el nuevo arreglo de
 * buckets. Esto es lo que mantiene insert/find/erase en O(1) *amortizado*
 * incluso cuando la tabla crece -- ver docs/guides/06_hash_tables.md para
 * el argumento completo.
 *
 * @tparam Key Tipo de la clave. Debe ser utilizable con `Hash` y
 * `operator==`.
 * @tparam Value Tipo del valor asociado. Debe ser construible por defecto
 * (usado por `operator[]`).
 * @tparam Hash Functor de hash, por defecto `std::hash<Key>`.
 */
template <typename Key, typename Value, typename Hash = std::hash<Key>>
class HashMap {
   public:
    using value_type = std::pair<Key, Value>;
    using bucket_type = std::vector<value_type>;

    /** @brief Umbral de factor de carga que dispara un rehash automático. */
    static constexpr double kMaxLoadFactor = 0.75;

    /** @brief Construye un mapa vacío con `bucket_count` buckets iniciales. */
    explicit HashMap(std::size_t bucket_count = 8)
        : buckets_(bucket_count == 0 ? 1 : bucket_count), size_(0) {}

    /**
     * @brief Inserta `key` con `value`, o sobrescribe el valor de una
     * entrada existente con la misma clave.
     * @return true si se insertó una nueva entrada, false si se sobrescribió
     * una existente.
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
     * @brief Accede al valor asociado a `key`, construyendo por defecto una
     * entrada para ella si aún no existe (como `std::unordered_map`).
     */
    Value& operator[](const Key& key) {
        (void)key;
        // TODO(estudiante): si `key` ya existe, devuelve una referencia a su
        // valor; si no, crea una entrada con Value{} en el bucket
        // correspondiente, incrementa size_ y devuelve una referencia a
        // ella. No olvides llamar a maybe_rehash() antes de insertar.
        throw std::logic_error("HashMap::operator[]: no implementado");
    }

    /** @brief Devuelve un puntero al valor de `key`, o nullptr si está ausente. */
    Value* find(const Key& key) {
        (void)key;
        // TODO(estudiante): recorre el bucket correspondiente a `key` y
        // devuelve un puntero a su valor si lo encuentras, o nullptr si no
        // está presente.
        throw std::logic_error("HashMap::find: no implementado");
    }

    /** @brief Sobrecarga const de find(). */
    const Value* find(const Key& key) const {
        (void)key;
        // TODO(estudiante): igual que la sobrecarga no-const, pero sobre un
        // bucket const y devolviendo un puntero const.
        throw std::logic_error("HashMap::find: no implementado");
    }

    /** @brief Devuelve una copia del valor de `key`, si está presente. */
    std::optional<Value> get(const Key& key) const {
        const Value* found = find(key);
        if (found == nullptr) return std::nullopt;
        return *found;
    }

    /** @brief Devuelve true si `key` está presente en el mapa. */
    bool contains(const Key& key) const { return find(key) != nullptr; }

    /**
     * @brief Elimina la entrada de `key`, si está presente.
     * @return true si se eliminó una entrada, false si `key` no se encontró.
     */
    bool erase(const Key& key) {
        (void)key;
        // TODO(estudiante): busca `key` en su bucket; si la encuentras,
        // elimínala del vector, decrementa size_ y devuelve true. Si no
        // está presente, devuelve false.
        throw std::logic_error("HashMap::erase: no implementado");
    }

    /** @brief Número de entradas clave-valor almacenadas en el mapa. */
    std::size_t size() const noexcept { return size_; }

    /** @brief True si el mapa no tiene entradas. */
    bool empty() const noexcept { return size_ == 0; }

    /** @brief Elimina todas las entradas y reduce a un solo bucket. */
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
        // reinserta cada entrada existente en el nuevo arreglo de buckets
        // (recalculando su índice con el nuevo bucket_count).
        throw std::logic_error("HashMap::maybe_rehash: no implementado");
    }

    std::vector<bucket_type> buckets_;
    std::size_t size_;
};

}  // namespace dsa

#endif  // DSA_HASHING_HASH_MAP_HPP
