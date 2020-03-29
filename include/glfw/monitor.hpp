#pragma once
/// \file monitor.hpp Monitor
#include <string>
#include <vector>
#include <glfw/config.hpp>
#include <glfw/gamma_ramp.hpp>
#include <glfw/error_handling.hpp>
#include <glfw/video_mode.hpp>

#include <glm/vec2.hpp>

namespace glfw {

struct monitor final {
  explicit operator bool() { return m_ != nullptr; }
  explicit monitor(c::GLFWmonitor* m) : m_{m} {}
  monitor(monitor const&) = default;
  monitor(monitor&&) = default;
  monitor& operator=(monitor const&) = default;
  monitor& operator=(monitor&&) = default;

  c::GLFWmonitor * get() { return m_; }
  c::GLFWmonitor const * get() const { return m_; }

  static monitor primary() noexcept {
    return monitor{detail::handle_glfw_error(
     c::glfwGetPrimaryMonitor(), "Coudln't find a primary monitor!")};
  }
  static std::vector<monitor> all() {
    int count;
    auto m_array = detail::handle_glfw_error(c::glfwGetMonitors(&count),
                                             "Couldn't find any monitors!");
    std::vector<monitor> monitors_;
    monitors_.reserve(count);
    for (int i = 0; i != count; ++i) {
      monitors_.emplace_back(monitor{m_array[i]});
    }
    return monitors_;
  }

  glfw::video_mode video_mode() const noexcept {
    return glfw::video_mode{c::glfwGetVideoMode(m_)};
  }

  const char * name() const noexcept {
    return detail::handle_glfw_error(c::glfwGetMonitorName(m_),
                                     "Couldn't get monitor name!");
  }

  std::vector<glfw::video_mode> video_modes() const noexcept {
    int count;
    auto vm_array = detail::handle_glfw_error(c::glfwGetVideoModes(m_, &count),
                                              "Couldn't find any video_modes!");
    std::vector<glfw::video_mode> vms_(count);
    for (int i = 0; i < count; ++i) {
      vms_.emplace_back(glfw::video_mode{vm_array + i});
    }
    return vms_;
  }

  const glfw::gamma_ramp gamma_ramp() const noexcept {
    return glfw::gamma_ramp{detail::handle_glfw_error(
     c::glfwGetGammaRamp(m_), "Couldn't get the gamma ramp")};
  }

  void set_gamma(float gamma) { c::glfwSetGamma(m_, gamma); }

  void set_gamma_ramp(glfw::gamma_ramp gamma_r) {
    c::GLFWgammaramp g;
    g.size = gamma_r.size();
    g.red = std::begin(gamma_r.red_range());
    g.green = std::begin(gamma_r.green_range());
    g.blue = std::begin(gamma_r.blue_range());
    c::glfwSetGammaRamp(m_, &g);
  }

  glm::ivec2 physical_size() const {
    glm::ivec2 size;
    c::glfwGetMonitorPhysicalSize(m_, &size.x, &size.y);
    return size;
  }

  glm::ivec2 position() const {
    glm::ivec2 pos;
    c::glfwGetMonitorPos(m_, &pos.x, &pos.y);
    return pos;
  }

  static void on_connect_or_disconnect_callback(c::GLFWmonitorfun callback) {
    detail::handle_glfw_error(c::glfwSetMonitorCallback(callback),
                              "Couldn't set the monitor callback");
  }

 private:
  c::GLFWmonitor* m_;
};

bool operator==(monitor const& a, monitor const& b) {
  return a.get() == b.get();
}

bool operator!=(monitor const& a, monitor const& b) { return !(a == b); }

}  // namespace glfw
