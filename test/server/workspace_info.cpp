#include <gtest/gtest.h>

#include <server/workspace_info.h>
#include <server/workspace_info_impl.h>

#include <cstdint>
#include <ranges>

namespace hydra::server {
  using Workspace = uint64_t;

  using WorkspaceInfo = WorkspaceInfoImpl<Workspace, struct Window>;

  struct Window {
    int id;
    WorkspaceInfo::Link workspace;
  };

  struct WorkspaceInfoTest: public testing::Test {
    Workspace workspace = 0;
    WorkspaceInfo w = workspace;
    std::vector<std::shared_ptr<Window>> windows;

    void SetUp() override {
      for(int i = 0; i < 10; i++) {
        windows.push_back(std::make_shared<Window>(i + 1));
      }
    }
  };

  TEST_F(WorkspaceInfoTest, FocusedLast) {
    // empty set, no focus
    ASSERT_EQ(w.focused_last(), nullptr);

    std::set<int> remaining;
    for(auto window: windows) {
      remaining.insert(window->id);
      w.place_new_window(window);
    }

    // focus is not null, even if focus not yet explicitly set
    {
      auto last = w.focused_last();
      ASSERT_NE(last, nullptr);
      ASSERT_GT(remaining.count(last->id), 0);
    }

    // explicitly set focus
    {
      auto focused = windows[windows.size() / 3];
      w.advise_focus_gained(focused);
      ASSERT_EQ(w.focused_last(), focused);
    }

    // focused_last(set) in subset
    {
      int lo = windows.size() / 2;
      auto last = w.focused_last(windows.begin() + lo, windows.end());
      ASSERT_NE(last, windows.end());
      ASSERT_GE((*last)->id, windows[lo]->id);
    }

    // Focused window in remaining set
    for(auto window: windows) {
      auto last = w.focused_last();
      ASSERT_NE(last, nullptr);
      ASSERT_GT(remaining.count(last->id), 0);

      remaining.erase(window->id);
      w.remove(window);
    }

    // empty set, no focus
    ASSERT_EQ(w.focused_last(), nullptr);
  }

  TEST_F(WorkspaceInfoTest, Push) {
    // no pending focus initially
    ASSERT_EQ(w.focus_pop(), nullptr);

    for(auto window: windows) {
      w.place_new_window(window);
    }

    // no pending focus initially
    ASSERT_EQ(w.focus_pop(), nullptr);

    // push/pop one window
    {
      auto target = windows[windows.size() / 2];
      w.focus_push(target);
      ASSERT_EQ(w.focus_pop(), target);
      ASSERT_EQ(w.focus_pop(), nullptr);
    }

    // pushing 2 windows overrides
    {
      auto target = windows[windows.size() / 2];
      w.focus_push(windows[0]);
      w.focus_push(target);
      ASSERT_EQ(w.focus_pop(), target);
      ASSERT_EQ(w.focus_pop(), nullptr);
    }

    // removing pending resets
    {
      auto target = windows[windows.size() / 2];
      w.focus_push(target);
      w.remove(target);
      ASSERT_EQ(w.focus_pop(), nullptr);
    }
  }
}
