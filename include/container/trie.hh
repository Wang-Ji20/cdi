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
#include "container/unordered_map.hh"
#include <memory>
#include <mutex>
#include <ostream>
#include <shared_mutex>

namespace cdi::container {

class Trie;
class TrieNodeGuard;

class TrieNode {
  friend class Trie;
  friend class TrieNodeGuard;

public:
  explicit TrieNode(char key) : key_(key) {}
  TrieNode(TrieNode &&movedTNode) noexcept
      : key_(movedTNode.key_), children_(std::move(movedTNode.children_)) {}
  virtual ~TrieNode() = default;
  virtual void Print(std::ostream &out, int depth = 0) const = 0;

protected:
  [[nodiscard("you must check the return value of me.")]] virtual auto
  HasValue() const -> bool = 0;

  [[nodiscard]] inline auto HasChild(char key) const -> bool {
    return children_.find(key) != children_.end();
  }

  [[nodiscard]] inline auto HasAnyChild() const -> bool {
    return !children_.empty();
  }

  [[nodiscard]] auto GetChild(char key) const
      -> cdi::constructor::Maybe<std::reference_wrapper<TrieNode>> {
    auto iter = children_.find(key);
    if (iter == children_.end()) {
      return cdi::constructor::none;
    }
    return *iter->second;
  }

  [[nodiscard]] auto GetChildGuardRead(char key) const
      -> cdi::constructor::Maybe<TrieNodeGuard>;

  [[nodiscard]] auto GetChildGuardWrite(char key)
      -> cdi::constructor::Maybe<TrieNodeGuard>;

  [[nodiscard]] inline auto GetKey() const -> char { return key_; }

  auto InsertKey(char key, std::unique_ptr<TrieNode> &&child)
      -> cdi::constructor::Maybe<std::reference_wrapper<TrieNode>> {
    auto iter = children_.find(key);
    if (iter != children_.end()) {
      return cdi::constructor::none;
    }
    children_.emplace(key, std::move(child));
    return this->GetChild(key);
  }

  [[nodiscard]] auto RemoveKey(char key) -> bool {
    auto iter = children_.find(key);
    if (iter == children_.end()) {
      return false;
    }
    children_.erase(iter);
    return true;
  }

  char key_;
  unordered_map<char, std::unique_ptr<TrieNode>> children_;

private:
  std::shared_mutex rwlatch_;
};

//===------------------------------------------------------------------------===
// TODO: finer grained lock
// Insert path:
//   shared lock the path to the node.
//   exclusive lock the insertion node.
//   insert the node.
//   release all locks.
// Delete path:
//   shared lock the path to the node.
//   exclusive lock the deletion node.
//   delete the node.
//   release all locks.
// Lookup path:
//   shared lock the path to the node.
//===------------------------------------------------------------------------===

/// This class is for higher concurrency, using finer grained lock.
class TrieNodeGuard {
public:
  TrieNodeGuard(TrieNode &node, bool readOnly = true)
      : node_(node), readOnly_(readOnly) {
    readOnly_ ? succeedGuard = node_.rwlatch_.try_lock_shared()
              : succeedGuard = node_.rwlatch_.try_lock();
  }

  ~TrieNodeGuard() {
    if (succeedGuard) {
      readOnly_ ? node_.rwlatch_.unlock_shared() : node_.rwlatch_.unlock();
    }
  }

  [[nodiscard]] auto Succeed() const -> bool { return succeedGuard; }

  [[nodiscard]] auto GetNode() const
      -> cdi::constructor::Maybe<std::reference_wrapper<TrieNode>> {
    if (!succeedGuard) {
      return cdi::constructor::none;
    }
    return node_;
  }

private:
  TrieNode &node_;
  bool readOnly_ = true;
  bool succeedGuard = false; // someones may not release the lock.
};

auto TrieNode::GetChildGuardRead(char key) const
    -> cdi::constructor::Maybe<TrieNodeGuard> {
  auto iter = children_.find(key);
  if (iter == children_.end()) {
    return cdi::constructor::none;
  }
  return TrieNodeGuard(*iter->second);
}

auto TrieNode::GetChildGuardWrite(char key)
    -> cdi::constructor::Maybe<TrieNodeGuard> {
  auto iter = children_.find(key);
  if (iter == children_.end()) {
    return cdi::constructor::none;
  }
  return TrieNodeGuard(*iter->second, false);
}

//===------------------------------------------------------------------------===
// Concrete TrieNode implementations
//===------------------------------------------------------------------------===

template <typename T> class ValueTrieNode : public TrieNode {
  friend class Trie;

public:
  explicit ValueTrieNode(char key, T value)
      : TrieNode(key), value_(std::move(value)) {}

  explicit ValueTrieNode(TrieNode &&movedTNode, T value)
      : TrieNode(std::move(movedTNode)), value_(std::move(value)) {}

private:
  [[nodiscard("you must check the return value of me.")]] inline auto
  HasValue() const -> bool override {
    return true;
  }

  [[nodiscard("you want my value, right? take it.")]] inline auto
  GetValue() const -> T {
    return value_;
  }

  void Print(std::ostream &out, int depth = 0) const override {
    out << std::string(depth, ' ') << key_ << " : " << value_ << std::endl;
    for (const auto &child : children_) {
      child.second->Print(out, depth + 1);
    }
  }

  T value_;
};

class NavigatorTrieNode : public TrieNode {
  friend class Trie;

public:
  explicit NavigatorTrieNode(char key) : TrieNode(key) {}

  explicit NavigatorTrieNode(TrieNode &&movedTNode)
      : TrieNode(std::move(movedTNode)) {}

private:
  void Print(std::ostream &out, int depth = 0) const override {
    out << std::string(depth, ' ') << key_ << std::endl;
    for (const auto &child : children_) {
      child.second->Print(out, depth + 1);
    }
  }

  [[nodiscard("you must check the return value of me.")]] auto HasValue() const
      -> bool override {
    return false;
  }
};

//===------------------------------------------------------------------------===
// Trie implementations
//===------------------------------------------------------------------------===
class Trie {
public:
  Trie() : root_(std::make_unique<NavigatorTrieNode>('\0')) {}
  ~Trie() = default;

  template <typename T> auto Insert(const std::string &key, T &value) -> bool {
    if (key.empty()) {
      return false;
    }

    std::scoped_lock<std::shared_mutex> lockInsert(rwlatch_);
    auto *currentNode = &root_;
    auto *nextNode = currentNode;
    for (char keychar : key) {
      // LESSON: use find() instead of operator[] to avoid segfault.
      if (auto queryResult = currentNode->get()->children_.find(keychar);
          queryResult != currentNode->get()->children_.end()) {
        nextNode = &queryResult->second;
      } else {
        currentNode->get()->InsertKey(
            keychar, std::make_unique<NavigatorTrieNode>(keychar));
        nextNode = &currentNode->get()->children_[keychar];
      }
      currentNode = nextNode;
    }

    // already has value, return false.
    if (currentNode->get()->HasValue()) {
      return false;
    }

    // currentNode is some NavigatorTrieNode
    *currentNode = std::make_unique<ValueTrieNode<T>>(std::move(**currentNode),
                                                      std::move(value));

    return true;
  }

  // True if key doesn't have a value
  // False if key not found.
  auto Remove(const std::string &key) -> bool {
    std::scoped_lock<std::shared_mutex> lockRemove(rwlatch_);
    return Remove(key, root_, root_);
  }

  /// Lookup a key in the trie.
  /// \param key The key to lookup.
  /// \param[out] value The value to store the result, if value is nullptr, then
  /// this method is used to check whether the key exists.
  /// \return true if the key exists, false otherwise.
  template <typename T>
  [[nodiscard("you must check whether the lookup succeed.")]] auto
  Lookup(const std::string &key, T *value = nullptr) -> bool {
    if (key.empty()) {
      return false;
    }

    auto queryResult = LookupMaybe<T>(key);
    if (!queryResult) {
      return false;
    }

    if (value) {
      *value = queryResult.value();
    }
    return true;
  }

  /// cdi-style api.
  template <typename T>
  auto LookupMaybe(const std::string &key) -> cdi::constructor::Maybe<T> {
    std::shared_lock<std::shared_mutex> readerlock(rwlatch_);

    // lockup the node
    auto queryResult = TraverseDown(key);
    if (!queryResult) {
      return constructor::none;
    }

    // see if the node has value
    auto *node = &queryResult->get();
    if (node->HasValue()) {
      return static_cast<ValueTrieNode<T> *>(node)->value_;
    }

    return constructor::none;
  }

  /// what can you expect from a function named `Print`...
  void Print(std::ostream &out) const {
    std::shared_lock<std::shared_mutex> readerlock(rwlatch_);
    root_->Print(out);
  }

private:
  // recursive remove
  auto Remove(const std::string &key,
              std::unique_ptr<TrieNode> &tnp,
              std::unique_ptr<TrieNode> &tnc) -> bool {
    // key empty, found & delete tn
    if (key.empty()) {
      if ((tnc->HasAnyChild())) {
        tnc = std::make_unique<NavigatorTrieNode>(std::move(*tnc));
      } else {
        (void)tnp->RemoveKey(tnc->GetKey());
      }
      return true;
    }

    // find next tn
    auto nextIter = tnc->children_.find(key[0]);
    if (nextIter == tnc->children_.end()) {
      return false;
    }
    auto &ntn = nextIter->second;

    // remove next tn
    if (!Remove(key.substr(1), tnc, ntn)) {
      return false;
    }

    // check if myself is orphan
    // root node can not remove
    if (!tnc->HasValue() && !tnc->HasAnyChild() && tnc != root_) {
      (void)tnp->RemoveKey(tnc->GetKey());
    }

    return true;
  }

  // NOT LOCKED
  auto TraverseDown(const std::string &key) const
      -> cdi::constructor::Maybe<std::reference_wrapper<TrieNode>> {
    auto *currentNode = root_.get();
    for (char keychar : key) {
      if (auto queryResult = currentNode->GetChild(keychar); queryResult) {
        currentNode = &queryResult->get();
        continue;
      }
      return cdi::constructor::none;
    }
    return *currentNode;
  }

  // TODO: TraverseDown with all read locks
  // TODO: TraverseDown with read locks and one mutex

  mutable std::shared_mutex
      rwlatch_; // TODO: alas, global lock is always a bad idea.
  std::unique_ptr<TrieNode> root_;
};

} // namespace cdi::container

#endif // CDI_CONTAINER_TRIE_HH
