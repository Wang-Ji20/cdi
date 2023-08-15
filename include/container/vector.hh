//===------------------------------------------===
// cdi 2023
//
// Identification: include/container/vector.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

//===------------------------------------------------------------------------===
// I want to make a memory safe vector.
// In current STL Container, when vector is empty, user call vector.front() or
// vector.back(), it segfaults and nobody know what happened.
//
// Here I let it crush, and print stack trace so that we know what happened.
//===------------------------------------------------------------------------===

#ifndef CDI_CONTAINER_VECTOR_HH
#define CDI_CONTAINER_VECTOR_HH

// #include "common/logger.hh"
#include <vector>

namespace cdi {
namespace container {

template <typename Tp> class vector : public std::vector<Tp> {
  using original = std::vector<Tp, std::allocator<Tp>>;
  using original::original;
  using size_type = typename original::size_type;
  using const_reference = typename original::const_reference;
  using reference = typename original::reference;

public:
  inline auto get(typename original::size_type _nn) ->
      typename original::reference {

    AssertIndexInBounds(_nn);

    return original::operator[](_nn);
  }

  inline auto get(typename original::size_type _nn) const ->
      typename original::const_reference {

    AssertIndexInBounds(_nn);

    return original::operator[](_nn);
  }

  auto operator[](typename original::size_type _nn) ->
      typename original::reference {
    return get(_nn);
  }
  auto operator[](typename original::size_type _nn) const ->
      typename original::const_reference {
    return get(_nn);
  }

  auto front() -> typename original::reference { return get(0); }

  auto front() const -> typename original::const_reference { return get(0); }

  auto back() -> typename original::reference {
    // CHECK(!original::empty());
    return get(original::size() - 1);
  }

  auto back() const -> typename original::const_reference {
    // CHECK(original::empty());
    return get(original::size() - 1);
  }

private:
  void AssertIndexInBounds(size_type index) {
    if (index >= original::size()) {
      // LOG(FATAL) << "vector index out of bound: " << index
      //            << " >= " << this->size();
    }
  }
};

} // namespace container

using container::vector;

} // namespace cdi

#endif // CDI_CONTAINER_VECTOR_HH
