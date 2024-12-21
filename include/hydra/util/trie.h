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
    struct node_data_t;
    template <typename T> struct base_node_t;

  public:
    using node_t = base_node_t<Trie*>;
    using const_node_t = base_node_t<const Trie*>;

    Trie() {
      nodes.resize(1);
    }

    template <typename... Sub>
    Trie(Sub const&... args): Trie() {
      auto child = root();
      (((child << args), false) || ...);
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
    struct base_node_t;

    struct node_data_t {
      Value v;
      std::map<Key, std::size_t> kids;
    };

    std::vector<node_data_t> nodes;
  };

  template <typename Key, typename Value>
  template <typename ptr_trie_t>
  struct Trie<Key, Value>::base_node_t {
  private:
    ptr_trie_t parent;
    std::size_t idx;

  protected:
    friend Trie<Key, Value>;
    base_node_t(ptr_trie_t parent, std::size_t idx): parent(parent), idx(idx) {}

  public:
    auto& self(this base_node_t self) { return self.parent->nodes[self.idx]; }

    std::optional<base_node_t> try_get(const Key& key) const {
      if(auto it = self().kids.find(key); it != self().kids.end()) {
        return base_node_t { parent, it->second };
      }

      return std::nullopt;
    }

    base_node_t operator[](const Key& key) {
      if(auto ret = try_get(key)) {
        return *ret;
      }

      return node_t { parent, self().kids[key] = parent->add() };
    }

    auto items() const {
      auto deref = [parent=this->parent] (const std::pair<Key, std::size_t>& p) {
        return std::pair<Key, const Value&>{ p.first, *base_node_t { parent, p.second } };
      };

      return self().kids | std::views::transform(deref);
    }

    template <typename... Sub>
    base_node_t operator<<(std::tuple<Key, Value, Sub...> const& args) {
      std::apply([this](Key const& key, Value const& value, Sub const&... args) {
        auto child = this->operator[](key);
        *child = value;
        (((child << args), false) || ...);
      }, args);
      return *this;
    }

    base_node_t operator<<(std::tuple<Key, Value> const& args) {
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
