#include <window_manager.h>

#include <miral/toolkit_event.h>
#include <miral/application_info.h>

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

  WindowSpecification WindowManager::place_new_window(ApplicationInfo const& app, WindowSpecification const& req) {
    auto res = Base::place_new_window(app, req);

    bool is_shell = shell_launcher->matches(app);
    auto& workspace = is_shell ? shell_workspace : default_workspace;
    auto metadata = std::make_shared<Metadata>(app, res);
    metadata->workspace = { workspace };

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

  void WindowManager::locked_list_windows(std::function<void(std::weak_ptr<Metadata>)> const& functor) {
    tools.invoke_under_lock([this,&functor]() {
      tools.for_each_window_in_workspace(default_workspace, [this,&functor](miral::Window const& win) {
        auto& win_info = tools.info_for(win);
        if(auto metadata = Metadata::try_from(tools, win_info)) {
          fmt::println("win {}", metadata->get_display_name());
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

  bool WindowManager::try_focus(MetadataPtr metadata) {
    if(auto win = metadata->try_window(tools)) {
      tools.select_active_window(win->window());
      return true;
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
