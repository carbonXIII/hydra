#include <backend/foreign_toplevel_wrapper.h>

namespace hydra::shell {
  ForeignTopLevels::ForeignTopLevels(wl_display* display)
    : self(RegistryListener{display, &zwlr_foreign_toplevel_manager_v1_interface, 2}.get_or_die<wl_resource>()) {

    self.setData(this);
    wrap(self, &Base::setToplevel, &ForeignTopLevels::create);
  }

  void ForeignTopLevels::create(wl_proxy* proxy) {
    toplevels.emplace_back(this, proxy).post_init(std::prev(toplevels.end()));
    for(auto& matcher: matchers) {
      bool after = matcher->predicate(toplevels.back());
      if(after) matcher->update(true, toplevels.back());
    }
  }

  void ForeignTopLevels::erase(ForeignTopLevels::iterator_t it) {
    for(auto& matcher: matchers) {
      bool before = matcher->predicate(*it);
      if(before) matcher->update(false, *it);
    }
    toplevels.erase(it);
  }

  struct ForeignTopLevels::Guard {
    ForeignTopLevels* parent;
    Toplevel* toplevel;
    std::vector<bool> before;
    Guard(Guard&&) = delete;
    Guard(ForeignTopLevels* parent, Toplevel* toplevel):
      parent(parent), toplevel(toplevel) {
      before.reserve(parent->matchers.size());
      for(auto& matcher: parent->matchers) {
        before.push_back(matcher->predicate(*toplevel));
      }
    }
    ~Guard() {
      unsigned i = 0;
      for(auto& matcher: parent->matchers) {
        bool after = matcher->predicate(*toplevel);
        if(after != before[i]) {
          matcher->update(after > before[i], *toplevel);
        }
        i++;
      }
    }
  };

  ForeignTopLevels::Guard ForeignTopLevels::guard(ForeignTopLevels::Toplevel* toplevel) {
    return Guard{this, toplevel};
  }

  void ForeignTopLevels::Toplevel::set_state(wl_array* _states) {
    auto g = parent->guard(this);
    auto states = std::span<zwlrForeignToplevelHandleV1State>{
      static_cast<zwlrForeignToplevelHandleV1State*>(_states->data),
      _states->size
    };

    this->state = {};
    for(auto state: states) {
      switch(state) {
        case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_FULLSCREEN:
          this->state.fullscreen = true;
          break;
        case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_ACTIVATED:
          this->state.active = true;
          break;
        default:
          break;
      }
    }
  }

  void ForeignTopLevels::Toplevel::set_title(const char* title) {
    auto g = parent->guard(this);
    this->title = title;
  }

  void ForeignTopLevels::Toplevel::closed() {
    parent->erase(it);
  }

  ForeignTopLevels::Toplevel::Toplevel(ForeignTopLevels* parent, wl_proxy* proxy): self(proxy), parent(parent) {
    self.setData(this);
    wrap(self, &Base::setState, &Toplevel::set_state);
    wrap(self, &Base::setTitle, &Toplevel::set_title);
    wrap(self, &Base::setClosed, &Toplevel::closed);
  }

  void ForeignTopLevels::Toplevel::post_init(ForeignTopLevels::iterator_t it) {
    this->it = it;
  }

  std::string_view ForeignTopLevels::Toplevel::get_title() { return title; }
  bool ForeignTopLevels::Toplevel::is_fullscreen() { return state.fullscreen; }
  bool ForeignTopLevels::Toplevel::is_active() { return state.active; }

  std::string to_string(ForeignTopLevels::Toplevel const& toplevel) {
    return fmt::format("Toplevel{{ '{}', fullscreen={}, active={}, }}", toplevel.title, toplevel.state.fullscreen, toplevel.state.active);
  }
}
