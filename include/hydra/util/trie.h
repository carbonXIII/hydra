#pragma once

#include <map>
#include <vector>
#include <optional>
#include <ranges>
#include <tuple>

namespace hydra::util {
  template <typename Key, typename Value>
  struct Trie {
  private:
    struct NodeData;
    template <typename T> struct BaseNode;

  public:
    using node_t = BaseNode<Trie*>;
    using const_node_t = BaseNode<Trie const*>;

    Trie() {
      nodes.resize(1);
    }

    template <typename... Sub>
    Trie(Sub const&... args): Trie() {
      auto child = root();
      (void) (((child << args), false) || ...);
    }

    node_t root() {
      return { this, 0 };
    }

    const_node_t croot() const {
      return { this, 0 };
    }

    std::size_t size() {
      return nodes.size();
    }

    node_t operator[](Key const& key) {
      return root()[key];
    }

  protected:
    std::size_t add() {
      auto ret = nodes.size();
      nodes.emplace_back();
      return ret;
    }

  private:
    template <typename ptr_trie_t>
    struct BaseNode;

    struct NodeData {
      Value v;
      std::map<Key, std::size_t> kids;
    };

    std::vector<NodeData> nodes;
  };

  template <typename Key, typename Value>
  template <typename ptr_trie_t>
  struct Trie<Key, Value>::BaseNode {
  private:
    ptr_trie_t parent;
    std::size_t idx;

  protected:
    friend Trie<Key, Value>;
    BaseNode(ptr_trie_t parent, std::size_t idx): parent(parent), idx(idx) {}

  public:
    auto& self(this BaseNode self) { return self.parent->nodes[self.idx]; }

    std::optional<BaseNode> try_get(Key const& key) const {
      if(auto it = self().kids.find(key); it != self().kids.end()) {
        return BaseNode { parent, it->second };
      }

      return std::nullopt;
    }

    BaseNode operator[](Key const& key) {
      if(auto ret = try_get(key)) {
        return *ret;
      }

      return node_t { parent, self().kids[key] = parent->add() };
    }

    auto items() const {
      auto deref = [parent=this->parent] (const std::pair<Key, std::size_t>& p) {
        return std::pair<Key, Value const&>{ p.first, *BaseNode { parent, p.second } };
      };

      return self().kids | std::views::transform(deref);
    }

    template <typename... Sub>
    BaseNode operator<<(std::tuple<Key, Value, Sub...> const& args) {
      std::apply([this](Key const& key, Value const& value, Sub const&... args) {
        auto child = this->operator[](key);
        *child = value;
        (void) (((child << args), false) || ...);
      }, args);
      return *this;
    }

    BaseNode operator<<(std::tuple<Key, Value> const& args) {
      std::apply([this](Key const& key, Value const& value) {
        auto child = this->operator[](key);
        *child = value;
      }, args);
      return *this;
    }

    auto& operator*() { return self().v; }
    auto* operator->() { return &self().v; }
  };

  template <typename Key, typename Value, typename... Sub>
  Trie(std::tuple<Key, Value, Sub> const&...) -> Trie<Key, Value>;
}
