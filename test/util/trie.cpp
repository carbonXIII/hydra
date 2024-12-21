#include <gtest/gtest.h>

#include <hydra/util/trie.h>

#include <fmt/core.h>

TEST(TrieTest, BinaryTree) {
  hydra::util::Trie<uint8_t, std::size_t> trie;

  ASSERT_EQ(trie.size(), 1);

  std::size_t created = 0;
  static constexpr int Bits = 8;
  static constexpr int Total = (1 << Bits);

  // this inserts many duplicates on purpose
  for(int i = 0; i < Total; i++) {
    auto cur = trie.root();
    for(int j = i, acc = 0, depth = 0; depth < Bits; j >>= 1, ++depth) {
      acc = acc * 2 + (j % 2);
      created += !cur.try_get(j % 2).has_value();
      cur = cur[j % 2];
      *cur = acc;
    }
  }

  ASSERT_EQ(trie.size(), 2 * Total - 1);
  ASSERT_EQ(trie.size(), created + 1);

  auto check = [](this auto self, auto cur, int depth, int acc) {
    if(*cur != acc) {
      return false;
    }

    if(depth == Bits) {
      return true;
    }

    auto left = cur.try_get(0);
    if(!left) return false;

    auto right = cur.try_get(1);
    if(!right) return false;

    return self(*left, depth + 1, acc * 2) && self(*right, depth + 1, acc * 2 + 1);
  };

  ASSERT_TRUE(check(trie.croot(), 0, 0));
  ASSERT_EQ(*trie[1][1][1][0][0], 28);
}

TEST(TrieTest, Initializer) {
  {
    hydra::util::Trie<char, std::size_t> trie;
    trie.root() << std::tuple{'c', std::size_t{0}}
      << std::tuple{'a', std::size_t{1}}
      << std::tuple{'t', std::size_t{2}};
    ASSERT_EQ(trie.size(), 4);
    ASSERT_EQ(*trie['c'], 0);
    ASSERT_EQ(*trie['a'], 1);
    ASSERT_EQ(*trie['t'], 2);
  }

  {
    hydra::util::Trie<char, std::size_t> trie{
      std::tuple{'c', std::size_t{0},
        std::tuple{'a', std::size_t{1}},
        std::tuple{'t', std::size_t{2}}
      }
    };
    ASSERT_EQ(trie.size(), 4);
    ASSERT_EQ(*trie['c'], 0);
    ASSERT_EQ(*trie['c']['a'], 1);
    ASSERT_EQ(*trie['c']['t'], 2);
  }
}
