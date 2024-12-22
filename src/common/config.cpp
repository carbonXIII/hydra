#include <hydra/config.h>

#include <utility>
#include <list>

#include <hydra/backend/sdl.h>

namespace hydra {
  struct Stack {
    std::mutex mtx;
    std::list<Config> stack;

    Config& peek() {
      return stack.back();
    }

    void push(Config const& config) {
      std::lock_guard<std::mutex> g{mtx};
      stack.emplace_back(config);
    }

    void pop() {
      std::lock_guard<std::mutex> g{mtx};
      stack.pop_back();
    }

    static Stack& Get() {
      static Stack ret;
      return ret;
    }

  protected:
    Stack(): stack({base_config}) {}
  };

  Config::Handle::Handle() = default;
  Config::Handle::Handle(bool active): active(active) {}

  Config::Handle& Config::Handle::operator=(Handle&& o) {
    if(active) {
      Stack::Get().pop();
    }

    active = std::exchange(o.active, false);
    return *this;
  }

  Config::Handle::~Handle() {
    if(active) {
      Stack::Get().pop();
    }
  };

  Config::Handle Config::Override(Config const& config) {
    Stack::Get().push(config);
    return Handle(true /* active */);
  }

  Config const& Config::Get() {
    return Stack::Get().peek();
  }
}
