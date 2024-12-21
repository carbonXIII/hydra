#pragma once

#include <miral/minimal_window_manager.h>
#include <miral/toolkit_event.h>
#include <miral/window_manager_tools.h>

#include <fmt/format.h>

#include <shell_launcher.h>
#include <workspace_info.h>

namespace hydra::server {
  struct WindowManager: miral::MinimalWindowManager {
    using Base = miral::MinimalWindowManager;

    struct Metadata;
    using MetadataPtr = std::shared_ptr<Metadata>;
    using WorkspaceInfo = WorkspaceInfoImpl<std::shared_ptr<miral::Workspace>, Metadata>;
    using WorkspaceLink = WorkspaceInfo::Link;

    WindowManager(miral::WindowManagerTools const& tools, ShellLauncher& shell);

    miral::WindowSpecification place_new_window(miral::ApplicationInfo const& app, miral::WindowSpecification const& req) override;
    void advise_new_window(miral::WindowInfo const& win) override;
    void advise_focus_gained(miral::WindowInfo const& win) override;
    void advise_delete_window(miral::WindowInfo const& win) override;

    virtual bool handle_keyboard_event(MirKeyboardEvent const* event) override;
    virtual void handle_modify_window(miral::WindowInfo& win, miral::WindowSpecification const& req) override;

    std::shared_ptr<miral::Workspace> active_workspace();
    auto info_for(std::shared_ptr<miral::Workspace> const& handle) -> WorkspaceInfo&;
    bool try_focus(MetadataPtr metadata, bool preserve_workspace = true);

    void locked_list_windows(std::function<void(std::weak_ptr<Metadata>)> const& functor);
    void locked_select_window(MetadataPtr metadata);

  private:
    ShellLauncher* shell_launcher;
    MetadataPtr shell_window;

    WorkspaceInfo default_workspace;
    WorkspaceInfo shell_workspace;
  };

  struct WindowManager::Metadata {
    Metadata(miral::ApplicationInfo const& app, miral::WindowSpecification const& spec);

    std::string get_display_name();

  protected:
    friend struct WindowManager;
    friend WorkspaceInfo;

    void update(miral::WindowManagerTools& tools, miral::WindowInfo const& win);
    static MetadataPtr try_from(miral::WindowManagerTools& tools, miral::WindowInfo const& win);
    miral::WindowInfo* try_window(miral::WindowManagerTools& tools) const;

    std::optional<std::string> id;
    WorkspaceLink workspace;

  private:
    std::string name;
    std::string app_name;
  };
}
