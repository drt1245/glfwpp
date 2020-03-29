#pragma once
/// \file window.hpp Window
#include <glfw/config.hpp>
#include <glfw/event_queue.hpp>
#include <glfw/environment.hpp>
#include <glfw/frame_size.hpp>
#include <glfw/frame_coordinates.hpp>
#include <glfw/monitor.hpp>
#include <glfw/window_attribute.hpp>

#include <memory>

namespace glfw {

struct window final {
  window(frame_size<int> s, const char * const title,
         glfw::monitor m = glfw::monitor{}, window * share = nullptr)
      : w{c::glfwCreateWindow(s.width, s.height, title, m.get(), share ? share->get() : nullptr), c::glfwDestroyWindow}, poll_events_{true}, wait_events_{false} {
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
    if (wait_events_) {
      wait_events();
    }
    if (poll_events_) {
      poll_events();
    }
    return !c::glfwWindowShouldClose(w.get());
  }

  auto get() noexcept -> c::GLFWwindow * { return w.get(); }
  auto get() const noexcept -> c::GLFWwindow const * { return w.get(); }

 private:
  std::unique_ptr<c::GLFWwindow, decltype(&c::glfwDestroyWindow)> w;
  event_queue* q;
  bool poll_events_;
  bool wait_events_;

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
      report_runtime_error("Couldn't get attribute " + to_string(a) + ".");
    }
    return v;
  }

  void make_current() noexcept {
    c::glfwMakeContextCurrent(w.get());
  }

  void set_wait_events(bool b) { wait_events_ = b; }
  void wait_events() const { c::glfwWaitEvents(); }

  void set_poll_events(bool b) { poll_events_ = b; }
  void poll_events() const { c::glfwPollEvents(); }

  void on_close(c::GLFWwindowclosefun callback) const noexcept {
    c::glfwSetWindowCloseCallback(w.get(), callback);
  }

  void swap_buffers() noexcept {
    c::glfwSwapBuffers(w.get());
  }

  void resize(frame_size<int> s) noexcept {
    c::glfwSetWindowSize(w.get(), s.width, s.height);
  }

  frame_size<int> size() const noexcept {
    frame_size<int> s_;
    c::glfwGetWindowSize(w.get(), &s_.width, &s_.height);
    return s_;
  }

  frame_size<int> framebuffer_size() const noexcept {
    frame_size<int> s_;
    c::glfwGetFramebufferSize(w.get(), &s_.width, &s_.height);
    return s_;
  }

  void position(frame_coordinates<int> p) noexcept {
    c::glfwSetWindowPos(w.get(), p.x, p.y);
  }

  frame_coordinates<int> position() const noexcept {
    frame_coordinates<int> pos;
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

  void mouse_position(frame_coordinates<double> p) noexcept {
    c::glfwSetCursorPos(w.get(), p.x, p.y);
  }

  frame_coordinates<double> mouse_position() const noexcept {
    frame_coordinates<double> p;
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
    auto b = q->pop(e);
    return b;
  }
};

bool operator==(window const& a, window const& b) { return a.get() == b.get(); }

bool operator!=(window const& a, window const& b) { return !(a == b); }

}  // namespace glfw
