#include <gtest/gtest.h>

#include <hydra/util/state_machine.h>
#include <hydra/util/state_machine_impl.h>

TEST(StateMachineTest, IdentityMachine) {
  struct BasicState {
    enum {
      IDENTITY
    };

    auto identity() {
      ++called;
      return [](std::size_t in) -> std::size_t {
        return IDENTITY;
      };
    }

    int called = 0;
  } state;

  auto sm = hydra::util::CreateStateMachine<
    std::size_t,
    BasicState,
    hydra::util::State{BasicState::IDENTITY, &BasicState::identity}
  >(&state);

  ASSERT_EQ(state.called, 0);
  ASSERT_EQ(sm->current(), 0);

  for(int i = 1; i <= 10; ++i) {
    (*sm)(0);
    ASSERT_EQ(state.called, i);
  }

  ASSERT_EQ(sm->current(), 0);

  sm->lock([&](std::size_t current) -> std::size_t {
    return -1;
  });

  ASSERT_EQ(sm->current(), 0);
}

struct Mod3 {
  template <std::size_t cur>
  auto go() {
    return [](std::size_t input) {
      return (cur * 10 + input) % 3;
    };
  }
};

TEST(StateMachineTest, Mod3) {
  Mod3 state;
  auto sm = hydra::util::CreateStateMachine<
    std::size_t,
    Mod3,
    hydra::util::State{0, &Mod3::go<0>},
    hydra::util::State{1, &Mod3::go<1>},
    hydra::util::State{2, &Mod3::go<2>}>(&state);

  std::string big_number = "353821467595972618";
  std::vector<std::size_t> expected = {0,2,2,1,0,1,2,2,0,2,2,1,1,2,1,1,2,1};

  ASSERT_EQ(sm->current(), 0);

  for(auto idx = 0; auto ch: big_number) {
    (*sm)(std::size_t(ch - '0'));
    ASSERT_EQ(sm->current(), expected[idx++]);
  }
}
