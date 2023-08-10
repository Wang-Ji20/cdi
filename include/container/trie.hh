//===------------------------------------------===
// cdi 2023
//
// Identification: include/container/trie.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#ifndef CDI_CONTAINER_TRIE_HH
#define CDI_CONTAINER_TRIE_HH

//===------------------------------------------------------------------------===
// This file implements a concurrent safe trie.
// I am considering RCU or COW version, but I'm not ready yet.
//
// A trie is a data structure for storing strings in a way which allows for fast
// prefix queries.  The basic idea is to store the string "foo" as a path
// through a tree, where each node is a single character.  For example, the
// string "foo" would be stored as three nodes, one for each character.  The
// string "bar" would share a node with "baz" for the first two characters.
//
// Classes: Trie, TrieNode
//
// TrieNode is a interface, representing a node in the trie.  It can be terminal
// or internal nodes. Two types.
//
// Trie provides most of our operations, containing:
// - Insert
// - Lookup
// - Delete
//
//===------------------------------------------------------------------------===

#include "constructor/maybe.hh"
#include <memory>
#include <shared_mutex>

namespace cdi::container {

class TrieNode;

class Trie {
public:
  Trie();
  ~Trie();

  template <typename T> auto Insert(const std::string &key, T &value) -> bool;

  auto Remove(const std::string &key) -> bool;

  /// Lookup a key in the trie.
  /// \param key The key to lookup.
  /// \param[out] value The value to store the result, if value is nullptr, then
  /// this method is used to check whether the key exists.
  /// \return true if the key exists, false otherwise.
  template <typename T>
  [[nodiscard("you must check whether the lookup succeed.")]] auto
  Lookup(const std::string &key, T *value) -> bool;

  /// cdi-style api.
  template <typename T>
  auto LookupMaybe(const std::string &key) -> cdi::constructor::Maybe<T>;

  void Print();

private:
  // recursive remove
  auto Remove(const std::string &key,
              std::unique_ptr<TrieNode> &tnp,
              std::unique_ptr<TrieNode> &tnc) -> bool;

  // NOT LOCKED
  auto TraverseDown(const std::string &key) const
      -> cdi::constructor::Maybe<std::reference_wrapper<TrieNode>>;

  // TODO: TraverseDown with all read locks
  // TODO: TraverseDown with read locks and one mutex

  mutable std::shared_mutex
      rwlatch_; // TODO: alas, global lock is always a bad idea.
  std::unique_ptr<TrieNode> root_;
};

} // namespace cdi::container

#endif // CDI_CONTAINER_TRIE_HH
