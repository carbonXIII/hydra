#include <hydra/util/state_machine.h>

#include <hydra/util/util.h>

namespace hydra::util {
  template <typename Input, typename T, auto... states>
  struct StateMachineImpl: StateMachine<Input> {
    using Base = StateMachine<Input>;
    using Node = std::variant<std::monostate, decltype(states)...>;
    using State = std::variant<std::monostate, std::invoke_result_t<decltype(states), T*>...>;

    static_assert((std::is_same_v<std::invoke_result_t<std::invoke_result_t<decltype(states), T*>, Input>, std::size_t> && ...));

    static constexpr auto table = []() {
      std::array<Node, sizeof...(states)> ret;
      ((ret[states.index] = states, false) || ...);
      return ret;
    }();

    static constexpr Node mono = std::monostate{};
    static constexpr Node const& get_node(std::size_t index) {
      if(index == StateMachine<Input>::npos || index > table.size()) {
        return mono;
      }
      return table[index];
    }

    T* t;
    State s;

    StateMachineImpl(T* t): t(t), s(std::in_place_index<1>) {}

    std::size_t current() const override {
      if(s.index() > 0) {
        return s.index() - 1;
      }

      return Base::npos;
    }

    void jump(std::size_t index) override {
      auto const& node = get_node(index);
      std::visit(overloaded {
          [](std::monostate) {},
            [this](auto&& node) {
              using Res = std::invoke_result_t<decltype(node), T*>;
              s.template emplace<Res>(std::invoke(node, t));
            },
            }, std::forward<decltype(node)>(node));
    }

    void next(Input input) override {
      std::visit(overloaded {
          [](std::monostate){},
            [this,input](auto& state) {
              std::size_t index = state.operator()(input);
              jump(index);
            },
            }, s);
    }
  };

  template <typename Input, typename T, auto... states>
  StateMachinePtr<Input> CreateStateMachine(auto&&... args) {
    return std::make_shared<StateMachineImpl<Input, T, states...>>(std::forward<decltype(args)>(args)...);
  }
}
