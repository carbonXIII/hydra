#pragma once

#include <variant>
#include <functional>
#include <memory>
#include <mutex>

#include <hydra/util/callback.h>

namespace hydra::util {
  template <typename Input>
  struct StateMachine: Callback<Input> {
    static constexpr std::size_t npos = -1;
    virtual std::size_t current() const = 0;

    void operator()(Input input) override {
      std::lock_guard g{mtx};
      next(input);
    }

    void lock(auto&& under_lock) {
      std::lock_guard g{mtx};
      jump(under_lock(current()));
    }

  protected:
    std::mutex mtx;
    virtual void next(Input input) = 0;
    virtual void jump(std::size_t index) = 0;
  };

  template <typename Input>
  using StateMachinePtr = std::shared_ptr<StateMachine<Input>>;

  template <typename Func>
  struct State {
    std::size_t index;
    Func func;
    auto operator()(auto* self) const { return std::invoke(func, self); }
  };

  template <typename Func>
  State(std::size_t, Func) -> State<Func>;

  template <typename Input, typename T, auto... states>
  StateMachinePtr<Input> CreateStateMachine(auto&&... args);
}
