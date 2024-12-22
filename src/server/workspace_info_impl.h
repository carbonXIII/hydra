#include "workspace_info.h"

#include <map>
#include <utility>

namespace hydra::server {
  template <typename Workspace, typename Window>
  WorkspaceInfoImpl<Workspace, Window>::WorkspaceInfoImpl(Workspace handle):
    handle(handle) {}

  template <typename Workspace, typename Window>
  void WorkspaceInfoImpl<Workspace, Window>::place_new_window(WorkspaceInfoImpl::WindowPtr& win) {
    windows.emplace_back(std::forward<decltype(win)>(win));
    win->workspace = {
      handle,
      std::prev(windows.end())
    };
  }

  template <typename Workspace, typename Window>
  void WorkspaceInfoImpl<Workspace, Window>::remove(WorkspaceInfoImpl::WindowPtr const& win) {
    if(auto link = win->workspace.pos) {
      if(is_pending_focus && link == windows.begin()) {
        is_pending_focus = false;
      }
      windows.erase(*link);
    }
  }

  template <typename Workspace, typename Window>
  void WorkspaceInfoImpl<Workspace, Window>::advise_focus_gained(WorkspaceInfoImpl::WindowPtr const& win) {
    if(auto link = win->workspace.pos; link && *link != windows.begin()) {
      windows.splice(windows.begin(), windows, *link);
    }
  }

  template <typename Workspace, typename Window>
  auto WorkspaceInfoImpl<Workspace, Window>::focused_last() -> WorkspaceInfoImpl::WindowPtr {
    return prune();
  }

  template <typename Workspace, typename Window>
  void WorkspaceInfoImpl<Workspace, Window>::focus_push(WorkspaceInfoImpl::WindowPtr const& win) {
    is_pending_focus = true;
    advise_focus_gained(win);
  }

  template <typename Workspace, typename Window>
  auto WorkspaceInfoImpl<Workspace, Window>::focus_pop() -> WorkspaceInfoImpl::WindowPtr {
    if(std::exchange(is_pending_focus, false)) {
      return focused_last();
    } else {
      return {};
    }
  }

  template <typename Workspace, typename Window>
  template <typename Iterator, typename Sentry>
  auto WorkspaceInfoImpl<Workspace, Window>::focused_last(Iterator begin, Sentry end) -> Iterator {
    std::map<WindowPtr, Iterator> m;

    auto it = begin;
    for(; it != end; ++it) {
      m[*it] = it;
    }

    prune();
    for(auto weak_window: windows) {
      if(auto it = m.find(weak_window.lock()); it != m.end()) {
        return it->second;
      }
    }

    return it;
  }

  template <typename Workspace, typename Window>
  auto WorkspaceInfoImpl<Workspace, Window>::prune() -> WindowPtr {
    WindowPtr ret;
    windows.remove_if([&ret](auto& weak) {
      if(auto win = weak.lock()) {
        if(!ret) ret = win;
        return false;
      } else {
        return true;
      }
    });
    return ret;
  }

  template <typename Workspace, typename Window>
  bool WorkspaceInfoImpl<Workspace, Window>::Link::operator==(Workspace const& o) {
    return workspace == o;
  }

  template <typename Workspace, typename Window>
  auto WorkspaceInfoImpl<Workspace, Window>::operator<=>(WorkspaceInfoImpl const& info) const { return handle<=>info.handle; }

  template <typename Workspace, typename Window>
  bool WorkspaceInfoImpl<Workspace, Window>::operator==(WorkspaceInfoImpl const& info) const { return handle == info.handle; }

  template <typename Workspace, typename Window>
  auto WorkspaceInfoImpl<Workspace, Window>::operator<=>(Link const& link) const { return handle<=>link.workspace; }

  template <typename Workspace, typename Window>
  bool WorkspaceInfoImpl<Workspace, Window>::operator==(Link const& link) const { return handle == link.workspace; }

}
