#pragma once

#include <wlr-foreign-toplevel-management-unstable-v1.hpp>

#include <list>
#include <memory>

#include <backend/wayland.h>

namespace hydra::shell {
  struct ForeignTopLevels {
    using Base = CCZwlrForeignToplevelManagerV1;

    struct Toplevel;
    friend Toplevel;

    ForeignTopLevels(wl_display* display);

    void add_matcher(auto&& predicate, auto&& update);

  protected:
    struct Guard;
    friend Guard;

    using list_t = std::list<Toplevel>;
    using iterator_t = list_t::iterator;

    Guard guard(Toplevel* toplevel);
    void erase(iterator_t it);

  private:
    struct Matcher;

    void create(wl_proxy* proxy);

    Base self;
    list_t toplevels;
    std::list<std::unique_ptr<Matcher>> matchers;
  };

  struct ForeignTopLevels::Toplevel {
    using Base = CCZwlrForeignToplevelHandleV1;

    Toplevel(ForeignTopLevels* parent, wl_proxy* proxy);

    std::string_view get_title();
    bool is_fullscreen();
    bool is_active();

    friend std::string to_string(ForeignTopLevels::Toplevel const& toplevel);

  protected:
    friend ForeignTopLevels;
    void post_init(ForeignTopLevels::iterator_t it);

  private:
    void set_state(wl_array* states);
    void set_title(const char* title);
    void closed();

    Base self;

    ForeignTopLevels* parent;
    ForeignTopLevels::iterator_t it;

    std::string title;
    struct {
      bool fullscreen = false;
      bool active = false;
    } state;
  };

  struct ForeignTopLevels::Matcher {
    virtual bool predicate(Toplevel& toplevel) { return false; }
    virtual void update(bool add, Toplevel& toplevel) {}
  };

  void ForeignTopLevels::add_matcher(auto&& predicate, auto&& update) {
    using Predicate = std::decay_t<decltype(predicate)>;
    using Update = std::decay_t<decltype(update)>;

    struct Impl: Matcher {
      virtual bool predicate(Toplevel& toplevel) override {
        return predicate_f(toplevel);
      }

      virtual void update(bool add, Toplevel& toplevel) override {
        return update_f(add, toplevel);
      }

      Predicate predicate_f;
      Update update_f;

      Impl(Predicate&& predicate, Update&& update):
        predicate_f(std::forward<decltype(predicate)>(predicate)),
        update_f(std::forward<decltype(update)>(update)) {}
    };

    matchers.emplace_back(std::make_unique<Impl>(std::forward<decltype(predicate)>(predicate),
                                                 std::forward<decltype(update)>(update)));
  }
}
