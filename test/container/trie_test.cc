//===------------------------------------------===
// cdi 2023
//
// Identification: test/container/trie_test.cc
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#include "container/trie.hh"
#include <gtest/gtest.h>
#include <iostream>

using namespace cdi::container;

TEST(TrieTest, Insert) {
  Trie trie;
  int value = 1;
  EXPECT_TRUE(trie.Insert<int>("foo", value));
  EXPECT_FALSE(trie.Insert<int>("foo", value));
  EXPECT_TRUE(trie.Insert<int>("bar", value));
  EXPECT_TRUE(trie.Insert<int>("baz", value));
  EXPECT_TRUE(trie.Insert<int>("foobar", value));
  trie.Print(std::cerr);
}

TEST(TrieTest, Remove) {
  Trie trie;
  int value = 1;
  EXPECT_TRUE(trie.Insert<int>("foo", value));
  EXPECT_TRUE(trie.Insert<int>("bar", value));
  EXPECT_TRUE(trie.Insert<int>("baz", value));
  EXPECT_TRUE(trie.Insert<int>("foobar", value));
  trie.Print(std::cerr);
  EXPECT_TRUE(trie.Remove("foo"));
  trie.Print(std::cerr);
  EXPECT_TRUE(trie.Remove("foo"));
  EXPECT_TRUE(trie.Remove("bar"));
  EXPECT_TRUE(trie.Remove("baz"));
  EXPECT_TRUE(trie.Remove("foobar"));
  trie.Print(std::cerr);
}

TEST(TrieTest, Lookup) {
  Trie trie;
  int value = 1;
  EXPECT_TRUE(trie.Insert<int>("foo", value));
  EXPECT_TRUE(trie.Insert<int>("bar", value));
  EXPECT_TRUE(trie.Insert<int>("baz", value));
  EXPECT_TRUE(trie.Insert<int>("foobar", value));
  EXPECT_TRUE(trie.Lookup<int>("foo"));
  EXPECT_TRUE(trie.Lookup<int>("bar"));
  EXPECT_TRUE(trie.Lookup<int>("baz"));
  EXPECT_TRUE(trie.Lookup<int>("foobar"));
  EXPECT_FALSE(trie.Lookup<int>("foob"));
  EXPECT_FALSE(trie.Lookup<int>("fooba"));
  EXPECT_FALSE(trie.Lookup<int>("foobaz"));
}
