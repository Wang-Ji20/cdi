//===------------------------------------------===
// cdi 2023
//
// Identification: lib/container/trie.cc
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#include "container/trie.hh"
#include "container/unordered_map.hh"
#include <mutex>

namespace cdi::container {

using namespace cdi::constructor;
using std::reference_wrapper;
using std::unique_ptr;

class Trie;
class TrieNodeGuard;

class TrieNode {
  friend class Trie;
  friend class TrieNodeGuard;

protected:
  explicit TrieNode(char key) : key_(key) {}
  ~TrieNode() = default;
  TrieNode(TrieNode &&movedTNode) noexcept
      : key_(movedTNode.key_), children_(std::move(movedTNode.children_)) {}

  [[nodiscard("you must check the return value of me.")]] virtual auto
  HasValue() const -> bool = 0;

  [[nodiscard]] inline auto HasChild(char key) const -> bool {
    return children_.find(key) != children_.end();
  }

  [[nodiscard]] inline auto HasAnyChild() const -> bool {
    return !children_.empty();
  }

  [[nodiscard]] auto GetChild(char key) const
      -> Maybe<reference_wrapper<TrieNode>> {
    auto iter = children_.find(key);
    if (iter == children_.end()) {
      return none;
    }
    return *iter->second;
  }

  [[nodiscard]] auto GetChildGuardRead(char key) const -> Maybe<TrieNodeGuard>;

  [[nodiscard]] auto GetChildGuardWrite(char key) -> Maybe<TrieNodeGuard>;

  [[nodiscard]] inline auto GetKey() const -> char { return key_; }

  [[nodiscard]] auto InsertKey(char key, unique_ptr<TrieNode> &&child)
      -> Maybe<reference_wrapper<TrieNode>> {
    auto iter = children_.find(key);
    if (iter != children_.end()) {
      return none;
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

  [[nodiscard]] auto GetNode() const -> Maybe<reference_wrapper<TrieNode>> {
    if (!succeedGuard) {
      return none;
    }
    return node_;
  }

private:
  TrieNode &node_;
  bool readOnly_ = true;
  bool succeedGuard = false; // someones may not release the lock.
};

auto TrieNode::GetChildGuardRead(char key) const -> Maybe<TrieNodeGuard> {
  auto iter = children_.find(key);
  if (iter == children_.end()) {
    return none;
  }
  return TrieNodeGuard(*iter->second);
}

auto TrieNode::GetChildGuardWrite(char key) -> Maybe<TrieNodeGuard> {
  auto iter = children_.find(key);
  if (iter == children_.end()) {
    return none;
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

  T value_;
};

class NavigatorTrieNode : public TrieNode {
  friend class Trie;

public:
  explicit NavigatorTrieNode(char key) : TrieNode(key) {}

  explicit NavigatorTrieNode(TrieNode &&movedTNode)
      : TrieNode(std::move(movedTNode)) {}

private:
  [[nodiscard("you must check the return value of me.")]] auto HasValue() const
      -> bool override {
    return false;
  }
};

//===------------------------------------------------------------------------===
// Trie implementations
//===------------------------------------------------------------------------===
using namespace std;

Trie::Trie() : root_(std::make_unique<NavigatorTrieNode>('\0')) {}

Trie::~Trie() = default;

auto Trie::TraverseDown(const std::string &key) const
    -> cdi::constructor::Maybe<std::reference_wrapper<TrieNode>> {
  auto *currentNode = root_.get();
  for (char keychar : key) {
    if (auto queryResult = currentNode->GetChild(keychar); queryResult) {
      currentNode = &queryResult->get();
      continue;
    }
    return none;
  }
  return *currentNode;
}

template <typename T>
auto Trie::Insert(const std::string &key, T &value) -> bool {
  if (key.empty()) {
    return false;
  }

  scoped_lock<shared_mutex> lockInsert(rwlatch_);
  auto *currentNode = root_.get();
  auto *nextNode = currentNode;
  for (char keychar : key) {
    if (auto queryResult = currentNode->GetChild(keychar); queryResult) {
      nextNode = &queryResult->get();
    } else {
      nextNode = &currentNode
                      ->InsertKey(keychar,
                                  std::make_unique<NavigatorTrieNode>(keychar))
                      ->get();
    }
    currentNode = nextNode;
  }

  // already has value, return false.
  if (currentNode->HasValue()) {
    return false;
  }

  // currentNode is some NavigatorTrieNode
  currentNode =
      make_unique<ValueTrieNode<T>>(std::move(*currentNode), std::move(value));

  return true;
}

template <typename T>
auto Trie::Lookup(const std::string &key, T *value) -> bool {
  if (key.empty()) {
    return false;
  }

  auto queryResult = LookupMaybe<T>(key);
  if (!queryResult) {
    return false;
  }

  if (value) {
    *value = queryResult->get();
  }
  return true;
}

template <typename T>
auto Trie::LookupMaybe(const std::string &key) -> cdi::constructor::Maybe<T> {
  shared_lock<shared_mutex> readerlock(rwlatch_);

  // lockup the node
  auto queryResult = TraverseDown(key);
  if (!queryResult) {
    return none;
  }

  // see if the node has value
  auto *node = &queryResult->get();
  if (node->HasValue()) {
    return static_cast<ValueTrieNode<T> *>(node)->value_;
  }

  return none;
}

auto Trie::Remove(const std::string &key) -> bool {
  scoped_lock<shared_mutex> lockRemove(rwlatch_);
  return Remove(key, root_, root_);
}

auto Trie::Remove(const std::string &key,
                  std::unique_ptr<TrieNode> &tnp,
                  std::unique_ptr<TrieNode> &tnc) -> bool {
  // key empty, found & delete tn
  if (key.empty()) {
    if ((tnc->HasAnyChild())) {
      tnc = make_unique<NavigatorTrieNode>(std::move(*tnc));
    } else {
      (void)tnp->RemoveKey(tnc->GetKey());
    }
    return true;
  }

  // find next tn
  auto *ntn = &tnc->children_[key[0]];
  if (ntn == nullptr) {
    return false;
  }

  // remove next tn
  if (!Remove(key.substr(1), tnc, *ntn)) {
    return false;
  }

  // check if myself is orphan
  // root node can not remove
  if (!tnc->HasValue() && !tnc->HasAnyChild() && tnc != root_) {
    (void)tnp->RemoveKey(tnc->GetKey());
  }

  return true;
}

} // namespace cdi::container
