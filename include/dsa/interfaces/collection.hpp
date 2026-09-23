#ifndef DSA_INTERFACES_COLLECTION_HPP
#define DSA_INTERFACES_COLLECTION_HPP

#include <cstddef>

namespace dsa {

/**
 * @brief Base contract every DSA container implements.
 *
 * Keeping this minimal on purpose: it only captures what every container
 * in this library has in common (a size and an emptiness check). Concrete
 * ADT contracts (List, Stack, Queue, Tree, ...) build on top of this.
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
