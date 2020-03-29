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
  window() : w{nullptr, c::glfwDestroyWindow} {}
  window(frame_size<int> s, const char * const title,
         glfw::monitor m = glfw::monitor{}, window const& share = window{})
      : w{c::glfwCreateWindow(s.width, s.height, title, m.get(), const_cast<c::GLFWwindow*>(share.get())), c::glfwDestroyWindow}, poll_events_{true}, wait_events_{false} {
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
    if(!w) {
      return false;
    }
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
  void assert_non_empty_window() const noexcept {
    assert(w && "action on empty window!");
  }

 public:
  static void hint(window_attribute target, int hint) noexcept {
    c::glfwWindowHint(
     static_cast<std::underlying_type_t<window_attribute>>(target), hint);
  }

  static void default_hints() noexcept { c::glfwDefaultWindowHints(); }

  int attribute(window_attribute a) const noexcept {
    assert_non_empty_window();
    auto v = c::glfwGetWindowAttrib(
     w.get(), static_cast<std::underlying_type_t<window_attribute>>(a));
    if (v == 0) {
      report_runtime_error("Couldn't get attribute " + to_string(a) + ".");
    }
    return v;
  }

  void make_current() noexcept {
    assert_non_empty_window();
    c::glfwMakeContextCurrent(w.get());
  }

  void set_wait_events(bool b) { wait_events_ = b; }
  void wait_events() const { c::glfwWaitEvents(); }

  void set_poll_events(bool b) { poll_events_ = b; }
  void poll_events() const { c::glfwPollEvents(); }

  bool is_closed() const noexcept {
    assert_non_empty_window();
    return w.get() != nullptr;
  }

  void on_close(c::GLFWwindowclosefun callback) const noexcept {
    assert_non_empty_window();
    c::glfwSetWindowCloseCallback(w.get(), callback);
  }

  void swap_buffers() noexcept {
    assert_non_empty_window();
    c::glfwSwapBuffers(w.get());
  }

  void resize(frame_size<int> s) noexcept {
    assert_non_empty_window();
    c::glfwSetWindowSize(w.get(), s.width, s.height);
  }

  frame_size<int> size() const noexcept {
    assert_non_empty_window();
    frame_size<int> s_;
    c::glfwGetWindowSize(w.get(), &s_.width, &s_.height);
    return s_;
  }

  frame_size<int> framebuffer_size() const noexcept {
    assert_non_empty_window();
    frame_size<int> s_;
    c::glfwGetFramebufferSize(w.get(), &s_.width, &s_.height);
    return s_;
  }

  void position(frame_coordinates<int> p) noexcept {
    assert_non_empty_window();
    c::glfwSetWindowPos(w.get(), p.x, p.y);
  }

  frame_coordinates<int> position() const noexcept {
    assert_non_empty_window();
    frame_coordinates<int> pos;
    c::glfwGetWindowPos(w.get(), &pos.x, &pos.y);
    return pos;
  }

  void title(const char* new_title) noexcept {
    assert_non_empty_window();
    c::glfwSetWindowTitle(w.get(), new_title);
  }

  void minimize() noexcept {
    assert_non_empty_window();
    c::glfwIconifyWindow(w.get());
  }
  void restore() noexcept {
    assert_non_empty_window();
    c::glfwRestoreWindow(w.get());
  }
  void show() noexcept {
    assert_non_empty_window();
    c::glfwShowWindow(w.get());
  }
  void hide() noexcept {
    assert_non_empty_window();
    c::glfwHideWindow(w.get());
  }

  void close() noexcept {
    assert_non_empty_window();
    w.reset(nullptr);
  }

  void mouse_position(frame_coordinates<double> p) noexcept {
    assert_non_empty_window();
    c::glfwSetCursorPos(w.get(), p.x, p.y);
  }

  frame_coordinates<double> mouse_position() const noexcept {
    assert_non_empty_window();
    frame_coordinates<double> p;
    c::glfwGetCursorPos(w.get(), &p.x, &p.y);
    return p;
  }

  glfw::monitor monitor() const noexcept {
    assert_non_empty_window();
    return glfw::monitor{c::glfwGetWindowMonitor(w.get())};
  }

  void make_context_current() noexcept {
    if (w) { make_current(); } else {
      c::glfwMakeContextCurrent(nullptr);
    }
  }

  bool poll(event& e) noexcept {
    auto b = q->pop(e);
    return b;
  }
};

bool operator==(window const& a, window const& b) { return a.get() == b.get(); }

bool operator!=(window const& a, window const& b) { return !(a == b); }

}  // namespace glfw
