#pragma once

#include <hydra/types.h>

namespace hydra::util {
  struct Callback {
    virtual void operator()(Option::value_t option) {}

    static auto Create(auto&& cb) {
      using CB = std::decay_t<decltype(cb)>;
      struct Inner: Callback {
        virtual void operator()(Option::value_t result) override {
          cb(result);
        }

        Inner(CB&& cb): cb(std::forward<decltype(cb)>(cb)) {}
        CB cb;
      };

      return Inner{std::forward<decltype(cb)>(cb)};
    }
  };
}
