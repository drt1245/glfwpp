#pragma once
/// \file window.hpp Window
#include <glfw/config.hpp>
#include <glfw/event_queue.hpp>
#include <glfw/environment.hpp>
#include <glfw/monitor.hpp>
#include <glfw/window_attribute.hpp>

#include <glm/vec2.hpp>
#include <memory>

namespace glfw {

struct window final {
  window(glm::ivec2 s, const char * const title,
         glfw::monitor *mon = nullptr, window * share = nullptr)
      : w{c::glfwCreateWindow(s.x, s.y, title, mon ? mon->get() : nullptr, share ? share->get() : nullptr), c::glfwDestroyWindow}, enable_poll_events{true}, enable_wait_events{false} {
    detail::handle_glfw_error(w.get(), "Couldn't create window!");
    q = environment::register_window(w.get());
    make_current();
  }

  ~window() noexcept {}

  window(window&&) = default;
  window& operator=(window&&) = default;

  window(window const&) = delete;
  window& operator=(window const&) = delete;

  explicit operator bool() {
    if (enable_wait_events) {
      wait_events();
    }
    if (enable_poll_events) {
      poll_events();
    }
    return !c::glfwWindowShouldClose(w.get());
  }

  c::GLFWwindow * get() noexcept { return w.get(); }
  c::GLFWwindow const * get() const noexcept { return w.get(); }

  bool enable_poll_events;
  bool enable_wait_events;

 private:
  std::unique_ptr<c::GLFWwindow, decltype(&c::glfwDestroyWindow)> w;
  event_queue* q;

 public:
  static void hint(window_attribute target, int hint) noexcept {
    c::glfwWindowHint(
     static_cast<std::underlying_type_t<window_attribute>>(target), hint);
  }

  static void default_hints() noexcept { c::glfwDefaultWindowHints(); }

  int attribute(window_attribute a) const noexcept {
    auto v = c::glfwGetWindowAttrib(
     w.get(), static_cast<std::underlying_type_t<window_attribute>>(a));
    if (v == 0) {
      std::string msg = std::string{"Couldn't get attribute "} + to_string(a) + ".";
      report_runtime_error(msg.c_str());
    }
    return v;
  }

  void make_current() noexcept {
    c::glfwMakeContextCurrent(w.get());
  }

  void wait_events() const { c::glfwWaitEvents(); }
  void poll_events() const { c::glfwPollEvents(); }

  void on_close(c::GLFWwindowclosefun callback) const noexcept {
    c::glfwSetWindowCloseCallback(w.get(), callback);
  }

  void swap_buffers() noexcept {
    c::glfwSwapBuffers(w.get());
  }

  void resize(glm::ivec2 s) noexcept {
    c::glfwSetWindowSize(w.get(), s.x, s.y);
  }

  glm::ivec2 size() const noexcept {
    glm::ivec2 s_;
    c::glfwGetWindowSize(w.get(), &s_.x, &s_.y);
    return s_;
  }

  glm::ivec2 framebuffer_size() const noexcept {
    glm::ivec2 s_;
    c::glfwGetFramebufferSize(w.get(), &s_.x, &s_.y);
    return s_;
  }

  void position(glm::ivec2 p) noexcept {
    c::glfwSetWindowPos(w.get(), p.x, p.y);
  }

  glm::ivec2 position() const noexcept {
    glm::ivec2 pos;
    c::glfwGetWindowPos(w.get(), &pos.x, &pos.y);
    return pos;
  }

  void title(const char* new_title) noexcept {
    c::glfwSetWindowTitle(w.get(), new_title);
  }

  void minimize() noexcept {
    c::glfwIconifyWindow(w.get());
  }
  void restore() noexcept {
    c::glfwRestoreWindow(w.get());
  }
  void show() noexcept {
    c::glfwShowWindow(w.get());
  }
  void hide() noexcept {
    c::glfwHideWindow(w.get());
  }

  void mouse_position(glm::dvec2 p) noexcept {
    c::glfwSetCursorPos(w.get(), p.x, p.y);
  }

  glm::dvec2 mouse_position() const noexcept {
    glm::dvec2 p;
    c::glfwGetCursorPos(w.get(), &p.x, &p.y);
    return p;
  }

  glfw::monitor monitor() const noexcept {
    return glfw::monitor{c::glfwGetWindowMonitor(w.get())};
  }

  void make_context_current() noexcept {
    make_current();
  }

  bool poll(event& e) noexcept {
    return q->pop(e);
  }
};

bool operator==(window const& a, window const& b) { return a.get() == b.get(); }

bool operator!=(window const& a, window const& b) { return !(a == b); }

}  // namespace glfw
