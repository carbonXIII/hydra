#pragma once

#include <memory>
#include <list>
#include <optional>

namespace hydra::server {
  template <typename Workspace,
            typename Window>
  struct WorkspaceInfoImpl {
    using WindowPtr = std::shared_ptr<Window>;

    WorkspaceInfoImpl(Workspace handle);
    WorkspaceInfoImpl(WorkspaceInfoImpl const&) = delete;
    WorkspaceInfoImpl(WorkspaceInfoImpl&&) = delete;

    void place_new_window(WindowPtr& win);
    void remove(WindowPtr const& win);
    void advise_focus_gained(WindowPtr const& win);

    void focus_push(WindowPtr const& win);
    WindowPtr focus_pop();
    WindowPtr focused_last();

    template <typename Iterator, typename Sentry>
    Iterator focused_last(Iterator begin, Sentry end);

    using List = std::list<std::weak_ptr<Window>>;
    struct Link {
      Workspace workspace;
      std::optional<typename List::iterator> pos;

      operator Workspace const&() const& {
        return workspace;
      }

      bool operator==(Workspace const& o);
    };

    auto operator<=>(WorkspaceInfoImpl const& info) const;
    bool operator==(WorkspaceInfoImpl const& info) const;
    auto operator<=>(Link const& link) const;
    bool operator==(Link const& link) const;

    operator Workspace const&() const& {
      return handle;
    }

  private:
    bool is_pending_focus = false;
    bool dirty = false;
    List windows;

    WindowPtr prune();

    Workspace const handle;
  };
}
