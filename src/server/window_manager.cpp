#include <window_manager.h>

#include <miral/toolkit_event.h>
#include <miral/application_info.h>

#include <utility>

#include <workspace_info_impl.h>

using namespace miral;

namespace hydra::server {
  using Metadata = WindowManager::Metadata;
  using MetadataPtr = WindowManager::MetadataPtr;

  WindowManager::WindowManager(WindowManagerTools const& tools, ShellLauncher& shell)
    : Base(tools),
      shell_launcher(&shell),
      default_workspace(this->tools.create_workspace()),
      shell_workspace(this->tools.create_workspace())
  {
    shell_launcher->set_window_manager(this);
  }

  bool WindowManager::handle_keyboard_event(MirKeyboardEvent const* event) {
    using namespace miral::toolkit;

    if(shell_launcher->handle_keyboard_event(event)) {
      return true;
    }

    return false;
  }

  void WindowManager::handle_modify_window(WindowInfo& win, WindowSpecification const& req) {
    Base::handle_modify_window(win, req);

    if(auto metadata = Metadata::try_from(tools, win);
       metadata && metadata == default_workspace.focused_last()) {
      shell_launcher->set_title(metadata->get_display_name());
    }
  }

  WindowSpecification WindowManager::place_new_window(ApplicationInfo const& app, WindowSpecification const& req) {
    auto res = Base::place_new_window(app, req);

    bool is_shell = shell_launcher->matches(app);
    auto& workspace = is_shell ? shell_workspace : default_workspace;
    auto metadata = std::make_shared<Metadata>(app, res);
    workspace.place_new_window(metadata);

    res.userdata() = metadata;
    if(is_shell) {
      shell_window = metadata;
    }

    return res;
  }

  void WindowManager::advise_new_window(WindowInfo const& win) {
    Base::advise_new_window(win);

    if(auto metadata = Metadata::try_from(tools, win)) {
      tools.add_tree_to_workspace(win.window(), metadata->workspace);
    }
  }

  void WindowManager::advise_focus_gained(WindowInfo const& _win) {
    auto& win = tools.info_for(_win.window());
    if(auto metadata = Metadata::try_from(tools, win)) {
      auto& workspace = info_for(metadata->workspace);
      if(auto pending = workspace.focus_pop();
         pending && try_focus(pending, false /* preserve workspace */)) {
      } else {
        Base::advise_focus_gained(win);
        workspace.advise_focus_gained(metadata);

        if(workspace == shell_workspace) {
          shell_launcher->set_title("");
        } else {
          shell_launcher->set_title(metadata->get_display_name());
        }
      }
    }
  }

  void WindowManager::advise_delete_window(WindowInfo const& win) {
    if(auto metadata = Metadata::try_from(tools, win)) {
      auto& workspace = info_for(metadata->workspace);
      workspace.remove(metadata);
      if(auto next_focus = workspace.focused_last()) {
        try_focus(next_focus, true /* preserve workspace */);
      } else if(workspace == active_workspace()) {
        shell_launcher->set_title("");
      }
    }

    Base::advise_delete_window(win);
  }

  void WindowManager::locked_list_windows(std::function<void(std::weak_ptr<Metadata>)> const& functor) {
    tools.invoke_under_lock([this,&functor]() {
      tools.for_each_window_in_workspace(default_workspace, [this,&functor](miral::Window const& win) {
        auto& win_info = tools.info_for(win);
        if(auto metadata = Metadata::try_from(tools, win_info)) {
          if(!win_info.can_be_active()) return;
          std::forward<decltype(functor)>(functor)(std::weak_ptr{metadata});
        }
      });
    });
  }

  void WindowManager::locked_select_window(MetadataPtr metadata) {
    tools.invoke_under_lock([this,&metadata](){
      try_focus(metadata);
    });
  }

  void WindowManager::locked_advance_window(const int steps) {
    tools.invoke_under_lock([this,steps]() {
      if(auto next = next_focused(steps)) {
        try_focus(next);
      }
    });
  }

  void WindowManager::locked_close_active() {
    tools.invoke_under_lock([this]{
      if(auto last_active = default_workspace.focused_last()) {
        if(auto win = last_active->try_window(tools)) {
          tools.ask_client_to_close(win->window());
        }
      }
    });
  }

  std::shared_ptr<Workspace> WindowManager::active_workspace() {
    const auto win = tools.active_window();
    if(auto metadata = Metadata::try_from(tools, tools.info_for(win))) {
      return metadata->workspace;
    }

    return nullptr;
  }

  auto WindowManager::info_for(std::shared_ptr<miral::Workspace> const& handle) -> WorkspaceInfo& {
    if(handle == shell_workspace) return shell_workspace;
    if(handle == default_workspace) return default_workspace;
    std::unreachable();
  }

  MetadataPtr WindowManager::next_focused(const int steps) {
    std::vector<MetadataPtr> windows;
    tools.for_each_window_in_workspace(default_workspace, [&](miral::Window const& win) {
      auto& win_info = tools.info_for(win);
      if(!win_info.can_be_active()) return;
      if(auto metadata = Metadata::try_from(tools, win_info)) {
        windows.push_back(metadata);
      }
    });

    if(windows.empty()) return nullptr;

    int cur = default_workspace.focused_last(windows.begin(), windows.end()) - windows.begin();

    int n = windows.size();
    cur = std::clamp(cur, 0, n - 1);
    int next = (cur + (steps % n) + n) % n;

    return windows[next];
  }

  bool WindowManager::try_focus(MetadataPtr metadata, bool preserve_workspace) {
    if(auto win = metadata->try_window(tools)) {
      bool can_select = tools.can_select_window(win->window());
      if(preserve_workspace && active_workspace() != metadata->workspace) {
        can_select = false;
      }

      if(!can_select) {
        info_for(metadata->workspace).focus_push(metadata);
        return false;
      } else {
        tools.select_active_window(win->window());
        return true;
      }
    }

    return false;
  }

  Metadata::Metadata(miral::ApplicationInfo const& app,
             miral::WindowSpecification const& spec)
      : name(spec.name().value_or("")),
        app_name(app.name()) {}

  std::string Metadata::get_display_name() {
    auto name_or = name.empty() ? "<untitled>" : name;
    if(app_name.empty()) {
      return name_or;
    }

    return fmt::format("{}: {}", app_name, name_or);
  }

  void Metadata::update(WindowManagerTools& tools, WindowInfo const& win) {
    name = win.name();
    app_name = tools.info_for(win.window().application()).name();
    id = tools.id_for_window(win.window());
  }

  MetadataPtr Metadata::try_from(WindowManagerTools& tools,
                                 WindowInfo const& win) {
    if(auto ret = std::reinterpret_pointer_cast<Metadata>(win.userdata())) {
      ret->update(tools, win);
      return ret;
    }

    return {};
  }

  WindowInfo* Metadata::try_window(WindowManagerTools& tools) const {
    if(id.has_value()) {
      return &tools.info_for_window_id(id.value());
    }

    return nullptr;
  }
}
