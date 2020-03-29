#pragma once
/// \file event.hpp Event
#include <glfw/config.hpp>
#include <glfw/event_type.hpp>
#include <glfw/key.hpp>
#include <glfw/modifier_keys.hpp>
#include <glfw/mouse_button.hpp>

#include <glm/vec2.hpp>

namespace glfw {

struct event {
  event_type type;
  explicit operator event_type() const { return type; }

 private:
  struct key_t {
    glfw::key key;
    glfw::modifier_keys modifiers;
    int scancode;
  };
  struct mouse_button_t {
    glfw::mouse_button button;
    glfw::modifier_keys modifiers;
  };

 public:
  union {
    key_t key;
    mouse_button_t mouse_button;
    glm::dvec2 mouse_wheel_offset;
    glm::dvec2 mouse_position;
    glm::ivec2 window_frame_size;
    glm::ivec2 window_position;
    glm::ivec2 window_size;
    unsigned int character;
  };
};

}  // namespace glfw
