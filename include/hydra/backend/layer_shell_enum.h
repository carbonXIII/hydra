#pragma once

#include <cstdint>

namespace hydra::shell {
  static const char* LAYER_SHELL_PROP_LAYER = "layer_shell.layer";
  static const char* LAYER_SHELL_PROP_ANCHORS = "layer_shell.anchors";
  static const char* LAYER_SHELL_PROP_EXCLUSIVE_ZONE = "layer_shell.exclusive_zone";

  enum Layer : uint32_t {
    LAYER_BACKGROUND = 0,
    LAYER_BOTTOM = 1,
    LAYER_TOP = 2,
    LAYER_OVERLAY = 3,
  };

  enum KeyboardInteractivity : uint32_t {
    KEYBOARD_INTERACTIVITY_NONE = 0,
    KEYBOARD_INTERACTIVITY_EXCLUSIVE = 1,
    KEYBOARD_INTERACTIVITY_ON_DEMAND = 2,
  };

  enum Anchor : uint32_t {
    ANCHOR_TOP = 1,
    ANCHOR_BOTTOM = 2,
    ANCHOR_LEFT = 4,
    ANCHOR_RIGHT = 8,
  };
}
