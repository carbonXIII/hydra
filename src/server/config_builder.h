#pragma once

#include <miral/configuration_option.h>

#include <array>
#include <utility>

#include <hydra/config.h>

namespace hydra::server {
  template <typename Value, typename T>
  struct ConfigOption: T {
    hydra::Config* config;
    std::string name;
    std::string desc;

    ConfigOption(std::string const& name,
                 std::string const& desc,
                 std::in_place_type_t<Value>, T&& t):
      T(std::forward<T&&>(t)), name(name), desc(desc) {}

    miral::ConfigurationOption operator()(hydra::Config& config, auto& callback) {
      this->config = &config;
      return miral::pre_init(miral::ConfigurationOption([this, &callback](Value value) {
        T::operator()(*this->config, value);
        callback();
      }, name, desc));
    }
  };

  template <typename Value, typename T, typename... Args>
  ConfigOption(std::in_place_type_t<Value>, T&&, Args const&...) -> ConfigOption<Value, T>;

  template <typename... T>
  struct ConfigBuilder: T... {
    int remaining = sizeof...(T);

    hydra::Config config;
    std::array<miral::ConfigurationOption, sizeof...(T)> options;
    hydra::Config::Handle handle;

    ConfigBuilder(T&&... t):
      T(std::forward<T&&>(t))...,
      config(hydra::Config::Get()),
      options{std::invoke([this](){ return T::operator()(config, *this); })...}
      {}

    void operator()() {
      if(--remaining == 0) {
        handle = hydra::Config::Override(config);
      }
    }

    void operator()(mir::Server& server) {
      for(auto& opt: options) {
        opt(server);
      }
    }
  };

  template <typename... T>
  ConfigBuilder(T...) -> ConfigBuilder<T...>;
}
